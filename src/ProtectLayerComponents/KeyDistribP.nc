/** 
 *  Component providing implementation of KeyDistrib interface.
 *  A module providing actual implementation of Key distribution component.
 * 	@version   1.0
 *  	@author Lukáš Němec
 * 	@date      2012-2014
 */
#include "ProtectLayerGlobals.h"

module KeyDistribP{
    /*@{*/
    uses {
        interface Crypto; /**< Crypto interface is used */
        interface SharedData;
        interface ResourceArbiter;
        interface Leds;
        interface Dispatcher;
    }
    provides {
        interface Init as PLInit;
        interface KeyDistrib; /**< KeyDistrib interface is provided */
    }
    /*@}*/
}
implementation{
//TODO clean up and add parameters checks
    
    PL_key_t* m_testKey;	/**< handle to key for selfTest */
    //PL_key_t preNeighKeys[MAX_NEIGHBOR_COUNT];
    uint8_t preKeysRetrieved = 0;
    uint8_t previousRetrieveStatus;
    static const char *TAG = "KeyDistP";

    //
    //	Init interface
    //
    command error_t PLInit.init() {
        pl_log_d(TAG, "<KeyDistribP.PLInit.init()>\n"); 
        call KeyDistrib.discoverKeys();
        
        
        pl_log_d(TAG, "</KeyDistribP.PLInit.init()>\n"); 
        return SUCCESS;
    }

    
    
    //
    //	KeyDistrib interface
    //
    
        
    task void retrievePrekeysTask() {
	//uint8_t status;
	combinedData_t* combData;
	SavedData_t* SavedData = NULL;
	
        pl_log_d(TAG, "retrievePrekeysTask posted.\n"); 

	combData = call SharedData.getAllData();
	SavedData = call SharedData.getSavedData();
	
	if(preKeysRetrieved == combData->actualNeighborCount){
#ifndef NO_CRYPTO
	      call Crypto.initCryptoIIB();
#endif
	      call Dispatcher.stateFinished(STATE_KEYDISTRIB_IN_PROGRESS);
	      
	} else {
	
	     call SharedData.getPredistributedKeyForNode(combData->savedData[preKeysRetrieved].nodeId, &(SavedData[preKeysRetrieved].kdcData.shared_key));	      
	}
    }
    
    event void ResourceArbiter.restoreKeyFromFlashDone(error_t result){
	if(result == SUCCESS){
	//key sucessfully retrieved, moving on to next one
	     preKeysRetrieved++;
	     previousRetrieveStatus = SUCCESS;
	     post retrievePrekeysTask();
	} else {
	     pl_log_e(TAG, "restoreKeyFromFlashDone failed.\n"); 
	     if(previousRetrieveStatus == SUCCESS){ 
	         //second attempt
	         previousRetrieveStatus = FAIL;
		 post retrievePrekeysTask();
	     } else {
		//second attempt failed, skipping this neighbor
	         //preNeighKeys[preKeysRetrieved] = NULL;
	         //TODO change variable name
	         preKeysRetrieved++;
	         previousRetrieveStatus = SUCCESS;
	         post retrievePrekeysTask();
	     }
	}
    }
    
    event void ResourceArbiter.saveCombinedDataToFlashDone(error_t result) {}
	
    event void ResourceArbiter.restoreCombinedDataFromFlashDone(error_t result) {}
    
    command error_t KeyDistrib.discoverKeys() {
	//post task for eeprom key retrieval
	previousRetrieveStatus = SUCCESS;
	post retrievePrekeysTask();
	return SUCCESS;
	/*
        error_t status = SUCCESS;

        pl_log_d(TAG, "<discoverKeys>.\n");
        if((status = call Crypto.initCryptoIIB()) != SUCCESS){
            pl_log_e(TAG, "discoverKeys failed.\n"); 
            return status;
        }
        pl_log_d(TAG, "</discoverKeys>.\n"); 
        */
    }

    command error_t KeyDistrib.getKeyToNodeB(uint8_t nodeID, PL_key_t** pNodeKey){
        SavedData_t* pSavedData = NULL;
        //pl_log_d(TAG, "getKeyToNodeB called for node '%u'\n", nodeID); 
	
        if(nodeID > NODE_MAX_ID || nodeID <= 0){
	    	pl_log_e(TAG, " invalid node ID.\n");
	    	return FAIL;
        }
        
        if(pNodeKey == NULL){
	    	pl_log_e(TAG, "pNodeKey NULL.\n");
	    	return FAIL;
        }
	
        pSavedData = call SharedData.getNodeState(nodeID);
        if (pSavedData != NULL) {
            *pNodeKey =  &((pSavedData->kdcData).shared_key);
            return SUCCESS;
        }
        else {
            //pl_log_e(TAG, "Failed to obtain SharedData.getNodeState.\n"); 
            return EKEYNOTFOUND;
        }
    }

    command error_t KeyDistrib.getKeyToBSB(PL_key_t** pBSKey) {
        KDCPrivData_t* KDCPrivData = NULL;

        if(pBSKey == NULL){
	    //pl_log_e(TAG, "pBSKey NULL.\n");
	    return FAIL;
        }

        //pl_log_d(TAG, "getKeyToBSB called.\n"); 
        KDCPrivData = call SharedData.getKDCPrivData();
        if(KDCPrivData == NULL){
            //pl_log_w(TAG, "getKeyToBSB key not received\n"); 
            return EKEYNOTFOUND;
        } else {		
            *pBSKey = &(KDCPrivData->keyToBS);
            return SUCCESS;
        }
    }

    command error_t KeyDistrib.getHashKeyB(PL_key_t** pHashKey) {
        KDCPrivData_t* KDCPrivData = NULL;

        if(pHashKey == NULL){
	    pl_log_e(TAG, "pHashKey NULL.\n");
	    return FAIL;
        }

        pl_log_d(TAG, "getHashKeyB called.\n");
        KDCPrivData = call SharedData.getKDCPrivData();
        if(KDCPrivData == NULL){
            pl_log_w(TAG, "getHashKeyB key not received\n");
            return EKEYNOTFOUND;
        } else {		
	    // set hash value to fixed initial value
            // BUGBUG: should be unknown to an attacker, now only zeroes
	    memset(KDCPrivData->hashKey.keyValue, 0, sizeof(KDCPrivData->hashKey.keyValue));
	    KDCPrivData->hashKey.counter = 0;
	    // return ptr to hash key structure	
            *pHashKey = &(KDCPrivData->hashKey);
	    
            return SUCCESS;
        }
    }

    command error_t KeyDistrib.selfTest(){
        uint8_t status = SUCCESS;
        /*
        status = call Crypto.selfTest();
        if(status == SUCCESS){
            call Leds.led1On();
        } else {
            call Leds.led2On();
        }
        */
        /*
        pl_log_d(TAG, "<Self test>\n"); 
        m_testKey = NULL;

        pl_log_d(TAG, "Self test getKeyToBS.\n"); 
        if((status = call KeyDistrib.getKeyToBSB(&m_testKey)) != SUCCESS){
            pl_log_e(TAG, "Self test getKeyToBS failed.\n"); 
            return status;
        }
        pl_log_d(TAG, "Self test getKeyToNodeB with ID 0.\n"); 
        if((status = call KeyDistrib.getKeyToNodeB( 0, &m_testKey)) != SUCCESS){
            pl_log_e(TAG, "Self test getKeyToNodeB failed.\n"); 
            return status;
        }
        pl_log_d(TAG, "</Self test>\n"); 
        */
        return status;
    }
    
#ifdef THIS_IS_BS
	event void Dispatcher.stateChanged(uint8_t newState) {
		//no code
	}
#endif
}

<?php

/**
 * @file
 * Moosend List API Wrapper.
 *
 */

/**
 * [moosend_get_active_lists_details description]
 * @param  integer $page      [description]
 * @param  integer $page_size [description]
 * @param  [type]  $list_id   [description]
 * @param  boolean $cache     [description]
 * @return [type]             [description]
 */
function moosend_get_active_lists_details($page = 1, $page_size = 10, $list_id = NULL, $cache = TRUE) {
  $lists_details = $lists_ids = array();
  if ($cache) {
     if (!empty(cache_get('lists', 'cache_moosend'))) {
      $lists_ids = cache_get('lists', 'cache_moosend') -> data;
      foreach ($lists_ids as $id) {
        $lists_details[$id] = cache_get('list-' . $id, 'cache_moosend')->data;
      }
      return !empty($list_id) ? $lists_details[$list_id] : $lists_details ;
     }
     else {
        moosend_get_active_lists_details($page, $page_size, $list_id, FALSE);
     }
  }
  try {
      $moosend_api_key = moosend_initialize_api();
      $active_mailing_lists = $moosend_api_key->mailingLists->getActiveMailingLists($page, $page_size);
      if (!empty($active_mailing_lists)) {
        foreach ($active_mailing_lists as $active_mailing_list) {
          $lists_details[$mailing_list_id]['ID'] = $mailing_list_id = $active_mailing_list->getID();
          $lists_details[$mailing_list_id]['Name'] =  $active_mailing_list->getName();
          $lists_details[$mailing_list_id]['ActiveMemberCount'] =  $active_mailing_list->getActiveMemberCount();
          $lists_details[$mailing_list_id]['BouncedMemberCount'] =  $active_mailing_list->getBouncedMemberCount();
          $lists_details[$mailing_list_id]['RemovedMemberCount'] =  $active_mailing_list->getRemovedMemberCount();
          $lists_details[$mailing_list_id]['UnsubscribedMemberCount'] =  $active_mailing_list->getUnsubscribedMemberCount();
          $lists_details[$mailing_list_id]['Status'] =  $active_mailing_list->getStatus();
          $lists_details[$mailing_list_id]['CreatedOn'] =  $active_mailing_list->getCreatedOn();
          $lists_details[$mailing_list_id]['UpdatedOn'] =  $active_mailing_list->getUpdatedOn();
          foreach ($active_mailing_list->getCustomFieldsDefinition() as $key => $field) {
            $lists_details[$mailing_list_id]['CustomFieldsDefinition'][$key] = (object)(array)$field; //Cast to stdClass object
          }
          cache_clear_all('list-' . $mailing_list_id, 'cache_moosend');
          cache_set('list-' . $mailing_list_id, $lists_details[$mailing_list_id], 'cache_moosend');
        }
        cache_clear_all('lists', 'cache_moosend');
        cache_set('lists', array_keys($lists_details), 'cache_moosend');
      }
    return !empty($list_id) ? $lists_details[$list_id] : $lists_details;
  } catch (Exception $e) {
    watchdog('moosend', 'An error occurred retrieving details for Mailing Lists."%message"', array(
    '%message' => $e->getMessage()),
    WATCHDOG_ERROR);
  }
}


/**
 * [moosend_get_active_list_custom_fields description]
 * @param  [type]  $list_id [description]
 * @param  boolean $cache   [description]
 * @return [type]           [description]
 */
function moosend_get_active_list_custom_fields($list_id, $cache = TRUE) {
  $fields = array();
  $active_list = moosend_get_active_lists_details(1, 10, $list_id, $cache);
  if (isset($active_list['CustomFieldsDefinition'])) {
    foreach ($active_list['CustomFieldsDefinition'] as $key => $field) {
      $fields[$field->Name] = (object)(array)$field; //Cast to stdClass object
    }
  }
  return $fields;
}


/**
 * [moosend_get_active_list_subscibers description]
 * @param  [type]  $list_id [description]
 * @param  [type]  $since   [description]
 * @param  boolean $cache   [description]
 * @return [type]           [description]
 *
 */
function moosend_get_active_list_subscibers($list_id, $since = NULL, $cache = FALSE ) {

  $subscribers = array();
  $since = isset($since) ? new DateTime($since) : NULL;

  if ( !strlen($list_id) ) {
    watchdog('moosend', 'An error occurred requesting subscribers. List ID can not be blank', NULL, WATCHDOG_ERROR);
    return NULL;
  }

  if ($cache) {
    $subscribers = cache_get($list_id . '-subscribers', 'cache_moosend')->data;
    return $subscribers;
  }

  $moosend_api = moosend_initialize_api();

  try {
    $members = $moosend_api->mailingLists->getSubscribers($list_id, 'Subscribed', $since);
    foreach ($members as $key => $subscriber) {
      $subscribers[$key]['name'] =  empty($subscriber->getName()) ?  t('Anonymous') : $subscriber->getName();
      $subscribers[$key]['email'] =  $subscriber->getEmail();
    }
    cache_set($list_id . '-subscribers', $subscribers, 'cache_moosend');
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred requesting subscribers for list @list. "%message"', array(
      '@list' => $list_id,
      '%message' => $e->getMessage(),
    ), WATCHDOG_ERROR);
  }
  return $subscribers;
}



/**
 * [moosend_create_mailing_list description]
 * @param  [type] $name                         [description]
 * @param  [type] $confirmation_page             [description]
 * @param  [type] $redirect_after_unsubscribe_page [description]
 * @return [type]                               [description]
 */
function moosend_create_mailing_list($name, $confirmation_page = NULL, $redirect_after_unsubscribe_page = NULL) {
   try {

    $moosend_api_key = moosend_initialize_api();

    $new_list_id = $moosend_api_key->mailingLists->create($name, $confirmation_page, $redirect_after_unsubscribe_page);
    watchdog('moosend', 'A new Mailing List with name "@name" was created',
        array('@name' => $name),
        WATCHDOG_NOTICE
      );
  }
  catch (Exception $e) {
      watchdog('moosend', 'An error occurred creating mailing list @name. "%message"', array(
        '@name' => $name,
        '%message' => $e->getMessage(),
      ), WATCHDOG_ERROR);
      return FALSE;
  }
  return $new_list_id;
}

/**
 * [moosend_delete_mailing_list description]
 * @param  [type] $list_id [description]
 * @return [type]          [description]
 */
function moosend_delete_mailing_list($list_id) {
    $status = TRUE;
    try {
    $moosend_api_key = moosend_initialize_api();
    $moosend_api_key->mailingLists->delete($list_id);
    watchdog('moosend', 'Mailing List with ID "@ID" deleted',
        array('@ID' => $list_id),
        WATCHDOG_NOTICE
      );
  }
  catch (Exception $e) {
      watchdog('moosend', 'An error occurred deleting mailing list @ID. "%message"', array(
        '@ID' => $list_id,
        '%message' => $e->getMessage(),
      ), WATCHDOG_ERROR);
      $status = FALSE;
  }
  return $status;
}


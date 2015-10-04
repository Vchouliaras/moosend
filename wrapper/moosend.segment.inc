<?php

/**
 * @file
 * Moosend Segment API Wrapper.
 *
 */


/**
 * [moosend_get_list_segments description]
 * @param  [type] $mailingListID [description]
 * @return [type]                [description]
 */
function moosend_get_list_segments($mailingListID){
  $segments = array();
  try {
    $moosend_api_key = initialize_moosend_api();
    $segmentsResponse = $moosend_api_key->segments->getSegments($mailingListID);
    foreach ($segmentsResponse as $key => $segment) {
      $segments[$key]['id'] = $segment->getID();
      $segments[$key]['name'] = $segment->getName();
    }
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred getting segments for list @list. "%message"', array(
      '@list' => $mailingListID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $segments;
}


/**
 * [moosend_get_list_segment_details description]
 * @param  [type] $mailingListID [description]
 * @param  [type] $segmentID     [description]
 * @return [type]                [description]
 */
function moosend_get_list_segment_details($mailingListID, $segmentID){
  $segment_details = array();
  $criteria = array();
  try {
    $moosend_api_key = initialize_moosend_api();
    $segmentDetailsResponse = $moosend_api_key->segments->getDetails($mailingListID, $segmentID);
    $segment_details['id'] = $segmentDetailsResponse->getID();
    $segment_details['name'] = $segmentDetailsResponse->getName();
    foreach ($segmentDetailsResponse->getCriteria() as $key => $value) {
      $criteria[$key]['id'] = $value->getID();
      $criteria[$key]['segment_id'] = $value->getSegmentID();
      $criteria[$key]['field'] = $value->getField();
      $criteria[$key]['custom_field_id'] = $value->getCustomFieldID();
      $criteria[$key]['field'] = $value->getField();
      $criteria[$key]['comparer'] = $value->getComparer();
      $criteria[$key]['value'] = $value->getValue();
      $criteria[$key]['date_from'] = $value->getDateFrom();
      $criteria[$key]['date_to'] = $value->getDateTo();
      $criteria[$key]['properties'] = $value->getProperties();
    }
    $segment_details['criteria'] = $criteria;
    $segment_details['match_type'] = $segmentDetailsResponse->getMatchType();
    $segment_details['created_by'] = $segmentDetailsResponse->getCreatedBy();
    $segment_details['created_on'] = _moosend_parse_date($segmentDetailsResponse->getCreatedOn());
    $segment_details['updated_by'] = $segmentDetailsResponse->getUpdatedBy();
    $segment_details['updated_on'] = _moosend_parse_date($segmentDetailsResponse->getUpdatedOn());
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred getting segment details for ID@segmentID. "%message"', array(
      '@segmentID' => $segmentID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $segment_details;
}


/**
 * [moosend_get_list_segment_subscribers description]
 * @param  [type]  $mailingListID [description]
 * @param  [type]  $segmentID     [description]
 * @param  string  $status        [description]
 * @param  integer $page          [description]
 * @param  integer $pagesize      [description]
 * @return [type]                 [description]
 */
function moosend_get_list_segment_subscribers($mailingListID, $segmentID, $status = 'Subscribed', $page = 1, $pagesize = 500 ){
  $subscribers = NULL;
  try {
    $moosend_api_key = initialize_moosend_api();
    $subscribers = $moosend_api_key->segments->getSubscribers($mailingListID, $segmentID, $status, $page, $pagesize);
  } catch (Exception $e) {
    watchdog('moosend', 'An error occurred getting subscribers for Segment ID @segmentID. "%message"', array(
      '@segmentID' => $segmentID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }
  return $subscribers;
}


/**
 * [moosend_create_segment description]
 * @param  [type] $mailingListID [description]
 * @param  [type] $name          [description]
 * @param  string $matchType     [description]
 * @return [type]                [description]
 */
function moosend_create_segment($mailingListID, $name, $matchType = 'All'){
  $createSegmentResponse = NULL;
  try {
    $moosend_api_key = initialize_moosend_api();
    $createSegmentResponse = $moosend_api_key->segments->create($mailingListID, $name, $matchType);
    watchdog('moosend', 'Segment  "@name" was created for list @list.',
      array('@name' => $name, '@list' => $mailingListID),
      WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred creating "@name" for list @list. "%message"', array(
      '@name' => $name,
      '@list' => $mailingListID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $createSegmentResponse;
}


/**
 * [moosend_update_segment description]
 * @param  [type] $mailingListID [description]
 * @param  [type] $name          [description]
 * @param  [type] $matchType     [description]
 * @return [type]                [description]
 */
function moosend_update_segment($mailingListID, $name, $matchType){
  $status = TRUE;
  try {
    $moosend_api_key = initialize_moosend_api();
    $updateSegmentResponse = $moosend_api_key->segments->update($mailingListID, $name, $matchType);
    watchdog('moosend', 'Segment  "@name" was updated for list @list.',
      array('@name' => $name, '@list' => $mailingListID),
      WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred updating "@name" for list @list. "%message"', array(
      '@name' => $name,
      '@list' => $mailingListID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * [moosend_delete_segment description]
 * @param  [type] $mailingListID [description]
 * @param  [type] $segmentID     [description]
 * @return [type]                [description]
 */
function moosend_delete_segment($mailingListID, $segmentID){
  $status = TRUE;
  try {
    $moosend_api_key = initialize_moosend_api();
    $segments_ids = array();

    foreach (moosend_get_list_segments($mailingListID) as $segment) {
        $segments_ids[] = $segment['id'];
    }

    if(in_array($segmentID, $segments_ids)){
      $deleteSegmentResponse = $moosend_api_key->segments->delete($mailingListID, $segmentID);
      watchdog('moosend', 'Segment  "@id" was deleted for list @list.',
        array('@name' => $segmentID, '@list' => $mailingListID),
        WATCHDOG_NOTICE);
      }
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred deleting "@name" for list @list. "%message"', array(
      '@name' => $name,
      '@list' => $mailingListID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $status;
}



/**
 * [moosend_add_criteria_to_segment description]
 * @param  [type] $mailingListID [description]
 * @param  [type] $segmentID     [description]
 * @param  [type] $field         [description]
 * @param  [type] $comparer      [description]
 * @param  [type] $value         [description]
 * @param  [type] $dateFrom      [description]
 * @param  [type] $dateTo        [description]
 * @return [type]                [description]
 */
function moosend_add_criteria_to_segment($mailingListID, $segmentID, $field, $comparer, $value, $dateFrom = NULL, $dateTo = NULL){
    $status = TRUE;
    $error_message = NULL;
    try {
      $moosend_api_key = initialize_moosend_api();
      $moosend_api_key->segments->addCriteria($mailingListID, $segmentID, $field, $comparer, $value, $dateFrom, $dateTo);

      watchdog('moosend', 'A new Criteria added to segment "@segment_id" for list @list.',
        array('@segment_id' => $segmentID, '@list' => $mailingListID),
        WATCHDOG_NOTICE);

    } catch (Exception $e) {
      $status = FALSE;

      watchdog('moosend', 'An error occurred adding criteria to segment "@segment_id" for list @list. "%message"', array(
      '@segment_id' => $segmentID,
      '@list' => $mailingListID,
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
      $error_message = $e->getMessage();

    }

    return array('status' => $status, 'error_message' => $error_message);
}



/**
 * Create Optgroup Select field
 * @param  boolean $append_mailing_list_fields [description]
 * @return [type]                              [description]
 */
function _moosend_criteria_fields($append_mailing_list_fields = TRUE){

  $criteria_fields = array( 'Custom Fields' =>
      drupal_map_assoc(array('DateAdded','RecipientName','RecipientEmail','CampaignsOpened','LinksClicked','CampaignName','LinkURL')),
    );

  if($append_mailing_list_fields){
    foreach (moosend_get_active_lists_details() as $key => $list) {
      foreach(moosend_get_active_list_custom_fields($key) as $field){
        $custom_fields[$field->ID] = $field->Name;
      }
      if(!empty($custom_fields)) {
        $criteria_fields['"'.$list['Name'] . '" Mailing List' ] = $custom_fields;
      }
    }
  }

  return $criteria_fields;
}



/**
 * [_moosend_criteria_comparer description]
 * @param  string $check_field [description]
 * @return [type]              [description]
 */
function _moosend_criteria_comparer($check_field = 'ALL'){
  switch ($check_field) {
    case 'DateAdded':
      $comparer = array('Is','IsNot','IsBefore','IsAfter');
      break;
    case 'RecipientName':
    case 'RecipientEmail':
      $comparer = array('Is','IsNot','Contains','DoesNotContain','StartsWith','DoesNotStartWith','EndsWith','DoesNotEndWith','IsEmpty','IsNotEmpty');
      break;
    case 'CampaignName':
      $comparer = array('Is','IsNot','Contains','DoesNotContain','StartsWith','DoesNotStartWith','EndsWith','DoesNotEndWith');
      break;
    case 'CampaignsOpened':
    case 'LinksClicked':
      $comparer = array('Is','IsNot','IsGreaterThan','IsGreaterThanOrEqualTo','IsLessThan','IsLessThanOrEqualTo');
      break;
    case 'LinkURL':
      $comparer = array('Is','IsNot');
      break;
    default:
      $comparer = array('Is','IsNot','Contains','DoesNotContain','StartsWith',
          'DoesNotStartWith','EndsWith','DoesNotEndWith','IsGreaterThan','IsGreaterThanOrEqualTo',
          'IsLessThan','IsLessThanOrEqualTo','IsBefore','IsAfter','IsEmpty','IsNotEmpty','IsTrue','IsFalse'
      );
      break;

    return drupal_map_assoc($comparer);
  }
}

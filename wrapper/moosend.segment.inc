<?php

/**
 * @file
 * Moosend Segment API Wrapper.
 */

/**
 * Get mailing list segments.
 */
function moosend_get_list_segments($mailing_list_id) {
  $segments = array();
  try {
    $moosend_api_key = moosend_initialize_api();
    $segments_response = $moosend_api_key->segments->getSegments($mailing_list_id);
    foreach ($segments_response as $key => $segment) {
      $segments[$key]['id'] = $segment->getID();
      $segments[$key]['name'] = $segment->getName();
    }
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred getting segments for list @list. "%message"', array(
      '@list' => $mailing_list_id,
      '%message' => $e->getMessage(),
    ),
      WATCHDOG_ERROR);
  }

  return $segments;
}


/**
 * Get specific segment details.
 */
function moosend_get_list_segment_details($mailing_list_id, $segment_id) {
  $segment_details = array();
  $criteria = array();
  try {
    $moosend_api_key = moosend_initialize_api();
    $segment_details_response = $moosend_api_key->segments->getDetails($mailing_list_id, $segment_id);
    $segment_details['id'] = $segment_details_response->getID();
    $segment_details['name'] = $segment_details_response->getName();
    foreach ($segment_details_response->getCriteria() as $key => $value) {
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
    $segment_details['match_type'] = $segment_details_response->getMatchType();
    $segment_details['created_by'] = $segment_details_response->getCreatedBy();
    $segment_details['created_on'] = _moosend_parse_date($segment_details_response->getCreatedOn());
    $segment_details['updated_by'] = $segment_details_response->getUpdatedBy();
    $segment_details['updated_on'] = _moosend_parse_date($segment_details_response->getUpdatedOn());
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred getting segment details for ID@segmentID. "%message"', array(
      '@segmentID' => $segment_id,
      '%message' => $e->getMessage(),
    ),
      WATCHDOG_ERROR);
  }

  return $segment_details;
}


/**
 * Get subcribers of specific segment.
 */
function moosend_get_list_segment_subscribers($mailing_list_id, $segment_id, $status = 'Subscribed', $page = 1, $pagesize = 500) {
  $subscribers = NULL;
  try {
    $moosend_api_key = moosend_initialize_api();
    $subscribers = $moosend_api_key->segments->getSubscribers($mailing_list_id, $segment_id, $status, $page, $pagesize);
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred getting subscribers for Segment ID @segmentID. "%message"', array(
      '@segmentID' => $segment_id,
      '%message' => $e->getMessage(),
    ),
      WATCHDOG_ERROR);
  }
  return $subscribers;
}


/**
 * Create segment for mailing list.
 */
function moosend_create_segment($mailing_list_id, $name, $match_type = 'All') {
  $create_segment_response = NULL;
  try {
    $moosend_api_key = moosend_initialize_api();
    $create_segment_response = $moosend_api_key->segments->create($mailing_list_id, $name, $match_type);
    watchdog('moosend', 'Segment  "@name" was created for list @list.',
      array('@name' => $name, '@list' => $mailing_list_id),
      WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    watchdog('moosend', 'An error occurred creating "@name" for list @list. "%message"', array(
      '@name' => $name,
      '@list' => $mailing_list_id,
      '%message' => $e->getMessage(),
    ),
      WATCHDOG_ERROR);
  }

  return $create_segment_response;
}

/**
 * Update segment for mailing list.
 */
function moosend_update_segment($mailing_list_id, $name, $match_type) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
    $moosend_api_key->segments->update($mailing_list_id, $name, $match_type);
    watchdog('moosend', 'Segment  "@name" was updated for list @list.',
      array('@name' => $name, '@list' => $mailing_list_id),
      WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred updating "@name" for list @list. "%message"', array(
      '@name' => $name,
      '@list' => $mailing_list_id,
      '%message' => $e->getMessage(),
    ),
      WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * Delete segment from mailing list.
 */
function moosend_delete_segment($mailing_list_id, $segment_id) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
    $segments_ids = array();

    foreach (moosend_get_list_segments($mailing_list_id) as $segment) {
      $segments_ids[] = $segment['id'];
    }

    if (in_array($segment_id, $segments_ids)) {
      $moosend_api_key->segments->delete($mailing_list_id, $segment_id);
      watchdog('moosend', 'Segment  "@id" was deleted for list @list.',
        array('@name' => $segment_id, '@list' => $mailing_list_id),
        WATCHDOG_NOTICE);
    }
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred deleting "@name" for list @list. "%message"', array(
      '@name' => $segment_id,
      '@list' => $mailing_list_id,
      '%message' => $e->getMessage(),
    ),
      WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * Add criteria to segment.
 */
function moosend_add_criteria_to_segment($mailing_list_id, $segment_id, $field, $comparer, $value, $date_from = NULL, $date_to = NULL) {
  $status = TRUE;
  $error_message = NULL;
  try {
    $moosend_api_key = moosend_initialize_api();
    $moosend_api_key->segments->addCriteria($mailing_list_id, $segment_id, $field, $comparer, $value, $date_from, $date_to);

    watchdog('moosend', 'A new Criteria added to segment "@segment_id" for list @list.',
      array('@segment_id' => $segment_id, '@list' => $mailing_list_id),
      WATCHDOG_NOTICE);

  }
  catch (Exception $e) {
    $status = FALSE;

    watchdog('moosend', 'An error occurred adding criteria to segment "@segment_id" for list @list. "%message"', array(
      '@segment_id' => $segment_id,
      '@list' => $mailing_list_id,
      '%message' => $e->getMessage(),
    ),
    WATCHDOG_ERROR);
    $error_message = $e->getMessage();

  }

  return array('status' => $status, 'error_message' => $error_message);
}


/**
 * Helper to Create Optgroup Select field.
 */
function _moosend_criteria_fields($append_mailing_list_fields = TRUE) {

  $criteria_fields = array(
    'Custom Fields' => drupal_map_assoc(
      array(
        'DateAdded',
        'RecipientName',
        'RecipientEmail',
        'CampaignsOpened',
        'LinksClicked',
        'CampaignName',
        'LinkURL',
      )
    ),
  );

  if ($append_mailing_list_fields) {
    foreach (moosend_get_active_lists_details() as $key => $list) {
      foreach (moosend_get_active_list_custom_fields($key) as $field) {
        $custom_fields[$field->ID] = $field->Name;
      }
      if (!empty($custom_fields)) {
        $criteria_fields['"' . $list['Name'] . '" Mailing List'] = $custom_fields;
      }
    }
  }

  return $criteria_fields;
}


/**
 * Helper to define segments criteria.
 */
function _moosend_criteria_comparer($check_field = 'ALL') {
  switch ($check_field) {
    case 'DateAdded':
      $comparer = array('Is', 'IsNot', 'IsBefore', 'IsAfter');
      break;

    case 'RecipientName':
    case 'RecipientEmail':
      $comparer = array(
        'Is',
        'IsNot',
        'Contains',
        'DoesNotContain',
        'StartsWith',
        'DoesNotStartWith',
        'EndsWith',
        'DoesNotEndWith',
        'IsEmpty',
        'IsNotEmpty',
      );
      break;

    case 'CampaignName':
      $comparer = array(
        'Is',
        'IsNot',
        'Contains',
        'DoesNotContain',
        'StartsWith',
        'DoesNotStartWith',
        'EndsWith',
        'DoesNotEndWith',
      );
      break;

    case 'CampaignsOpened':
    case 'LinksClicked':
      $comparer = array(
        'Is',
        'IsNot',
        'IsGreaterThan',
        'IsGreaterThanOrEqualTo',
        'IsLessThan',
        'IsLessThanOrEqualTo',
      );
      break;

    case 'LinkURL':
      $comparer = array('Is', 'IsNot');
      break;

    default:
      $comparer = array(
        'Is',
        'IsNot',
        'Contains',
        'DoesNotContain',
        'StartsWith',
        'DoesNotStartWith',
        'EndsWith',
        'DoesNotEndWith',
        'IsGreaterThan',
        'IsGreaterThanOrEqualTo',
        'IsLessThan',
        'IsLessThanOrEqualTo',
        'IsBefore',
        'IsAfter',
        'IsEmpty',
        'IsNotEmpty',
        'IsTrue',
        'IsFalse',
      );
      break;
  }

  return drupal_map_assoc($comparer);
}

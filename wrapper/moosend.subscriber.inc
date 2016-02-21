<?php

/**
 * @file
 * Moosend Segment API Wrapper.
 */

/**
 * Search subscriber in mailing list.
 */
function moosend_is_a_subscriber($list_id, $email, $cache = FALSE) {
  $subscribers = moosend_get_active_list_subscibers($list_id, NULL, $cache);
  $emails = array_column($subscribers, 'email');/* >= PHP 5.5.0*/

  return in_array($email, $emails);
}


/**
 * Define Moosend Operations.
 */
function _moosend_member_operations() {
  return drupal_map_assoc(array('Subscribe', 'Unsubscribe', 'Remove'));
}


/**
 * Pass operations through this function to support cron jobs.
 */
function moosend_pre_operate_member() {
  if (count(func_get_args()) === 1) {
    $args = func_get_args();
    $args = reset($args);
  }
  else {
    $args = func_get_args();
  }
  $operation_value = end($args);

  if (in_array($operation_value, _moosend_member_operations())) {
    $function = strtolower('moosend_' . $operation_value . '_member');

    // Remove operation argument.
    array_pop($args);

    if (variable_get('moosend_use_cron', FALSE)) {
      return moosend_add_to_cron_queue($function, $args);
    }
    return call_user_func_array($function, $args);
  }
}


/**
 * Subcribe member to mailing list.
 */
function moosend_subscribe_member($list_id, $email, $name, $custom_fields = array()) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
    $member = moosend_subscriber_params($email, $name, $custom_fields);

    $moosend_api_key->subscribers->addSubscriber($list_id, $member);
    cache_clear_all($list_id . '-subscribers', 'cache_moosend');

    watchdog('moosend', '@email was subscribed to list @list.',
        array('@email' => $email, '@list' => $list_id),
        WATCHDOG_NOTICE
      );
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred subscribing @email to list @list. "%message"', array(
      '@email' => $email,
      '@list' => $list_id,
      '%message' => $e->getMessage(),
    ), WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * Unsubcribe member from mailing list.
 */
function moosend_unsubscribe_member($list_id, $email, $campaing_id = NULL) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();

    $moosend_api_key->subscribers->unsubscribe($email, $campaing_id, $list_id);
    cache_clear_all($list_id . '-subscribers', 'cache_moosend');

    watchdog('moosend', '@email was unsubscribe from list @list.',
       array('@email' => $email, '@list' => $list_id),
       WATCHDOG_NOTICE
     );
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred unsubscribe @email from list @list. "%message"', array(
      '@email' => $email,
      '@list' => $list_id,
      '%message' => $e->getMessage(),
    ), WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * Remove member from mailing list.
 */
function moosend_remove_member($list_id, $email) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();

    $moosend_api_key->subscribers->removeSubscriber($list_id, $email);
    cache_clear_all($list_id . '-subscribers', 'cache_moosend');

    watchdog('moosend', '@email was removed from list @list.',
       array('@email' => $email, '@list' => $list_id),
       WATCHDOG_NOTICE
     );
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred removing @email from list @list. "%message"', array(
      '@email' => $email,
      '@list' => $list_id,
      '%message' => $e->getMessage(),
    ), WATCHDOG_ERROR);
  }
  return $status;
}


/**
 * Get subscriber parameters.
 */
function moosend_subscriber_params($email, $name, $custom_fields) {
  $member = NULL;
  $fields = array();
  $custom_fields = !empty($custom_fields) ? $custom_fields : array();
  $member = new moosend\Models\SubscriberParams();
  $member->email = $email;
  $member->name = $name;
  if (!empty($custom_fields)) {
    foreach ($custom_fields as $key => $field) {
      // Create fields with proper format.
      $fields[] = $key . '=' . $field;
    }
  }

  $member->customFields = $fields;
  return $member;
}

<?php

/**
 * @file
 * Rules integration for the Moosend Mailing Lists.
 */


/**
 * Implements hook_rules_action_info().
 *
 */
function moosend_lists_rules_action_info() {
  $items = array();
  $items['moosend_lists_rules_member_operations'] = array(
    'parameter' => array(
      'user' => array(
        'type' => 'user',
        'label' => t('User'),
        'description' => t('The user to operate against in Mailing Lists.'),
      ),
      'operation' => array(
        'type' => 'text',
        'label' => t('Operations'),
        'options list' => '_moosend_member_operations',
        'description' => t('The Moosend operation to perform for the user.'),
        'restriction' => 'input'
      ),
      'mailing_list' => array(
        'type' => 'text',
        'label' => t('Moosend Mailing Lists'),
        'options list' => '_moosend_lists_rules_mailing_lists',
        'description' => t('Choose the Mailing List'),
        'restriction' => 'input'
      ),
    ),
    'group' => t('Moosend'),
    'label' => t('Subscribe/Unsubscribe/Remove Moosend members'),
    'access callback' => 'moosend_lists_rules_actions_access_callback',
  );

  return $items;
}


/**
 * Access callback for the rules condition.
 *
 */
function moosend_lists_rules_actions_access_callback() {
  return user_access('administer moosend');
}


/**
 *  Options list callback;
 *  Get Moosend Mailing Lists
 *
 */
function _moosend_lists_rules_mailing_lists() {
  foreach (moosend_get_active_lists_details() as $list_id => $list_details) {
    $lists[$list_id] = $list_details['Name'];
  }

  return $lists;
}


/**
 * Rules Ui form callback
 *
 */
function moosend_lists_rules_member_operations_form_alter(&$form, &$form_state, $options, RulesAbstractPlugin $element) {
    $form['parameter']['mailing_list']['settings']['mailing_list']['#ajax'] = rules_ui_form_default_ajax('fade');
    $form['parameter']['operation']['settings']['operation']['#ajax'] = rules_ui_form_default_ajax('fade');
}


/**
 * Rules Action alter callback
 *
 */
function moosend_lists_rules_member_operations_info_alter(&$element_info, $element) {
  //Remove all predefine custom criteria
  foreach ($element_info['parameter'] as $key => &$parameter) {
    if (strpos($key, 'custom_field_') !== FALSE) {
      unset($element_info['parameter'][$key]);
    }
  }
  if ($element->settings['operation'] === 'Subscribe') {
    $selected_list_custom_fields = moosend_get_active_list_custom_fields($element->settings['mailing_list']);
    if (!empty($selected_list_custom_fields)) {
      foreach ($selected_list_custom_fields as $key => $field) {
        $element_info['parameter']['custom_field_' . $field->Name] = array(
          'type' => 'text',
          'label' => t('Custom field "@name"', array('@name' => $field->Name)),
          'optional' => ($field->IsRequired) ? FALSE : TRUE,
        );
      }
    }
    //@TODO Check for custom cirteria Type
  }
}


/**
 * Action callback;
 * Operate to Moosend Member
 *
 */
function moosend_lists_rules_member_operations($user, $operation, $mailing_list) {
  //Get custom fields from arguments
  $args = func_get_args();
  $custom_fields = array();
  foreach ($args as $key => $argument) {
    if (is_object($argument)) {
      if (get_class($argument) === 'RulesState') {
        foreach ($argument->currentArguments as $key => $value) {
          $string = 'custom_field_';
          if (strpos($key, $string) !== FALSE) {
            $custom_fields[ substr($key, strlen($string)) ] = $value;
          }
        }
      }
    }
  }
  if (!empty($user->uid)) {
    if ($operation === 'Subscribe') {
      $args = array(
        $mailing_list,
        $user->mail,
        $user->name,
        $custom_fields,
        $operation
      );
    }
    else {
      $args = array(
        $mailing_list,
        $user->mail,
        $operation
      );
    }
  moosend_pre_operate_member($args);
  }
}

<?php


function moosend_subscriptions_form($form,&$form_state,$subscriptions = NULL){

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('The title for this form.'),
    '#size' => 35,
    '#maxlength' => 32,
    '#default_value' => isset($subscriptions->name) ? $subscriptions->name : '',
    '#required' => TRUE,
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => isset($subscriptions->settings['description']) ? $subscriptions->settings['description'] : '',
    '#rows' => 2,
    '#maxlength' => 500,
    '#description' => t('This description will be shown below the signup form.'),
   );

  // Create a fieldset that will be included in the vertical tab.
   $form['moosend_vertical_tabs'] = array(
    '#type' => 'vertical_tabs'
    );

    $form['display_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Pick a display mode'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#weight' => -99,
      '#group' => 'moosend_vertical_tabs',
    );

    $modes = array(
      MOOSEND_SUBSCRIPTIONS_BLOCK_DISPLAY => array(MOOSEND_SUBSCRIPTIONS_BLOCK_DISPLAY),
      MOOSEND_SUBSCRIPTIONS_PAGE_DISPLAY => array(MOOSEND_SUBSCRIPTIONS_PAGE_DISPLAY),
      MOOSEND_SUBSCRIPTIONS_BOTH_PAGE_BLOCK => array(
        MOOSEND_SUBSCRIPTIONS_BLOCK_DISPLAY,
        MOOSEND_SUBSCRIPTIONS_PAGE_DISPLAY
        ),
      );

    $form['display_settings']['mode'] = array(
      '#type' => 'checkboxes',
      '#description' => t('Select a display mode for your subscription form'),
      '#required' => TRUE,
      '#default_value' => isset($subscriptions->mode) ? $modes[$subscriptions->mode] : array(),
      '#options' => array(
        MOOSEND_SUBSCRIPTIONS_BLOCK_DISPLAY => t('As a Block'),
        MOOSEND_SUBSCRIPTIONS_PAGE_DISPLAY => t('As a Page')
        ),

      );

    $form['list_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Select a Moosend list'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => -98,
      '#group' => 'moosend_vertical_tabs',
    );

    $lists_details = moosend_get_active_lists_details(array(),FALSE);
    $options = array();
    foreach ($lists_details as $key => $list) {
      $options[$key] = $list['Name'];
    }

    $form['list_settings']['lists'] = array(
      '#type' => 'select',
      '#description' => t('Select a list to attach your subscription form.'),
      '#required' => TRUE,
      '#options' => $options,
      '#default_value' => isset($subscriptions->list) ? $subscriptions->list : '',
      '#ajax' => array(
        'callback' => 'moosend_subscriptions_custom_fields_callback',
        'wrapper' => 'custom-fields-wrapper',
        'method' => 'replace',
        'effect' => 'fade',
        'progress' => array(
          'type' => 'throbber',
          'message' => t('Getting custom fields...'),
          ),
        )
      );

    $form['list_settings']['custom_fields'] = array(
        '#prefix' => '<div id="custom-fields-wrapper">',
        '#suffix' => '</div>',
        '#group' => 'moosend_vertical_tabs',
    );

    if ( !empty($form_state['values']['lists']) || isset($subscriptions->list)) {

      $form['list_settings']['custom_fields'] = array(
        '#type' => 'fieldset',
        '#tree' => TRUE,
        '#id' => 'custom-fields-wrapper',
        '#title' => t('Custom Fields Display'),
        '#description' => t('Select the custom fields to show on subscription  forms. Required fields are automatically selected.'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      $selected_list_key = !empty($form_state['values']['lists']) ? $form_state['values']['lists'] : $subscriptions->list ;
      $selected_list_custom_fields = moosend_get_active_list_custom_fields($selected_list_key);
      if (!empty($selected_list_custom_fields)) {
            foreach ($selected_list_custom_fields as $key => $field) {
              $form['list_settings']['custom_fields'][$field->Name] = array(
                '#type' => 'checkbox',
                '#title' => $field->Name,
                '#default_value' => var_export($field->IsRequired,TRUE) === 'true' ? 1 : '',
                '#required' => var_export($field->IsRequired,TRUE) === 'true' ? TRUE : FALSE,
                '#disabled' => var_export($field->IsRequired,TRUE) === 'true' ? TRUE : FALSE,
            );
          }
        }
        else{
            $form['list_settings']['custom_fields'] = array(
              '#prefix' => '<div id="custom-fields-wrapper">'.t('No specified fields for this list.'),
              '#suffix' => '</div>',
          );
        }
      }

     $form['customize_subscriptions'] = array(
      '#type' => 'fieldset',
      '#title' => t('Customize the Form'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#weight' => -97,
      '#group' => 'moosend_vertical_tabs',
    );
     $path_prefix = url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q=');

     $form['customize_subscriptions']['page_url'] = array(
      '#type' => 'textfield',
      '#size' => 35,
      '#title' => t('Subscription Page Url'),
      '#maxlength' => 80,
      // '#required' => TRUE,
      '#description' => t('Path to the subscription page. ie "newsletter/subscribe".'),
      '#default_value' => isset($subscriptions->settings['page_url']) ? $subscriptions->settings['page_url'] : '',
      '#field_prefix' => $path_prefix,
      '#states' => array(
        'visible' => array(   // action to take.
          ':input[id=edit-display-settings-mode-2]' =>  array('checked' => TRUE),
        ),
      ),
    );

    $form['customize_subscriptions']['confirm_message'] = array(
      '#type' => 'textfield',
      '#size' => 35,
      '#title' => t('Confirm Message'),
      '#maxlength' => 100,
      '#description' => t('This message will appear after a successful submission of this form.'),
      '#default_value' => isset($subscriptions->settings['confirm_message']) ? $subscriptions->settings['confirm_message'] : '',
    );

    $form['customize_subscriptions']['redirect_page'] = array(
      '#type' => 'textfield',
      '#size' => 35,
      '#title' => t('Redirect Page'),
      '#maxlength' => 80,
      '#description' => t('Redirect user to a confirmation page. Leave blank to stay on same page.'),
      '#default_value' => isset($subscriptions->settings['redirect_page']) ? $subscriptions->settings['redirect_page'] : '',
      '#field_prefix' => $path_prefix,
    );

    $form['actions']['save'] = array(
      '#type' => 'submit',
      '#value' => isset($subscriptions->sub_id) ? t('Update') : t('Save'),
    );

    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#access' => isset($subscriptions),
      '#submit' => array('moosend_subscriptions_form_delete'),
    );

  return $form;
}


/**
 *  Ajax Callback
 *
 *  Select custom fields
 *
 */
function moosend_subscriptions_custom_fields_callback($form,$form_state){
    return $form['list_settings']['custom_fields'];
}


/**
 * Submit handler for subscriptions_form().
 *
 */
function moosend_subscriptions_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $subscriptions = !isset($form_state['moosend_subscriptions']) ? entity_get_controller('moosend_subscriptions')->create() : $form_state['moosend_subscriptions'];

  $subscriptions->name = $values['title'];
  $subscriptions->mode  =array_sum($values['display_settings']['mode']);
  $subscriptions->list = $values['lists'];
  $subscriptions->settings['description'] = $values['description'];
  $subscriptions->settings['custom_fields'] = array();
  if (isset($values['custom_fields'])) {
    $active_list_fields = moosend_get_active_list_custom_fields($subscriptions->list);
    foreach ($values['custom_fields'] as $key => $field) {
      if ((int)$field) {
        $subscriptions->settings['custom_fields'][$key] = $active_list_fields[$key];
      }
    }
  }
  if(!empty($values['customize_subscriptions']['page_url'])){
     $subscriptions->settings['page_url']   = $values['customize_subscriptions']['page_url'];
     menu_rebuild();
  }
  $subscriptions->settings['confirm_message'] = $values['customize_subscriptions']['confirm_message'];
  $subscriptions->settings['redirect_page'] = $values['customize_subscriptions']['redirect_page'];
  if ($subscriptions->save()){
     drupal_set_message(t('Subscription form @label has been saved successfully.',
      array('@label' => $subscriptions->name)),'status', FALSE);
     $form_state['redirect'] = 'admin/config/services/moosend/subscriptions';
  }
  else{
    drupal_set_message(t('There has been an error trying to save the form.'), 'error');
  }
}


function moosend_subscriptions_form_delete($form, &$form_state){
  $form_state['redirect'] = 'admin/config/services/moosend/subscriptions/' . $form_state['subscriptions']->identifier() . '/delete';
}

<?php

/**
* @file
*  Moosend Lists Module for Admin Interface.
*
*/

/**
 * Page Callback: Display the Moosend Lists page
 *
 * @return array
 */
function moosend_lists_admin_page() {

    //Retrieve list from moosend_cache
    $lists = moosend_get_active_lists_details();

    $domain_prefix = variable_get('moosend_user_domain');

    $header = array(
      t('Mailing List'), t('Active Subscribers'), t('Unsubscribed'), t('Custom Fields'), t('Segments'), t('Operations')
    );

    if (empty($lists)) {
      $message = t('Your mailing lists are empty. Click the link bellow or go to !moosend and create some mailing lists',
        array('!moosend' => l(t('Moosend'), 'https://' . $domain_prefix . '.moosend.com/',
            array('attributes' => array('target' => '_blank'), 'fragment' => '/mailing-lists')
          )
        )
      );
      drupal_set_message($message, 'warning', FALSE);
      //Render Table
      return $build = array(
          'mailing_list_table' => array(
            '#theme' => 'table',
            '#header' => $header,
        ),
      );
    }

    $rows = array();
    foreach ($lists as $list_id => $list_detail) {

     //Get CustomFieldDefinitions
    $custom_fields_definitions = array();
     if (!empty($list_detail['CustomFieldsDefinition'])) {
        foreach ($list_detail['CustomFieldsDefinition'] as $key => $field) {
          $custom_fields_definitions[] = $field->Name;
        }
      $custom_fields_definitions = implode(', ', $custom_fields_definitions);
     }

     //Get Lists Segments
     $segments = moosend_get_list_segments($list_id);
     if (!empty($segments)) {
      $list_segements = array();
      foreach ($segments as $key => $segment) {
        $list_segements[] = l($segment['name'], 'https://' . $domain_prefix . '.moosend.com/',
            array(
              'attributes' => array('target' => '_blank'),
              'fragment' => '/mailing-lists/' . $list_id . '/segments/'
            )
        );
      }
      $render_segments = implode(', ', $list_segements);
     }
     else {
      $render_segments = t('None');
     }


     //Print table Rows
     $rows[$list_id]['data'] = array(
        $list_detail['Name'],
        $list_detail['ActiveMemberCount'],
        $list_detail['UnsubscribedMemberCount'],
        !empty($custom_fields_definitions) ? $custom_fields_definitions : t('None'),
        $render_segments,
        //Attributes on the cell
        l(t('Edit'), 'https://' . $domain_prefix . '.moosend.com/',
            array(
              'attributes' => array('target' => '_blank'),
              'fragment' => '/mailing-lists/' . $list_id . '/view-list',
            )
          ) . ' | ' .
          l(t('Delete'), 'admin/config/services/moosend/lists/delete/' . $list_id,
            array(
              'query' => array('destination' => 'admin/config/services/moosend/lists')
            )
          ),
      );
    }

    // Initialize Pager
    $current_page = pager_default_initialize(count($rows), MOOSEND_PAGER);

    // Split list into page sized chunks
    $chunks = array_chunk($rows, MOOSEND_PAGER, TRUE);

    $build = array(
        'mailing_list_table' => array(
          '#theme' => 'table',
          '#header' => $header,
          '#rows' => $chunks[$current_page],
        ),
        'refresh_link' => array(
          '#type' => 'markup',
          '#markup' => l(t('Refresh Mailing Lists'),
            'admin/config/services/moosend/lists/clear_cache',
            array('query' => array('destination' => 'admin/config/services/moosend/lists'))
          )
        ),
        'moosend_pager' => array(
          '#theme' => 'pager',
          '#quantity' => count($rows),
        )
      );

    return $build;
}


/**
 * Menu Callback
 *
 * Moosend List cache clear form.
 *
 */
function moosend_lists_clear_cache($form, &$form_state) {
  $cache_clear_destination = 'admin/config/services/moosend/lists';
  return confirm_form($form,
    t('Reset Moosend mailing lists'),
    $cache_clear_destination,
    t('Confirm clearing of Moosend mailing list cache.'),
    'Confirm'
  );
}


/**
 * Handler for lists cache clear form.
 *
 */
function moosend_lists_clear_cache_submit($form, &$form_state) {
  moosend_get_active_lists_details(1, 10, NULL, FALSE);
  drupal_set_message(t('Moosend Lists cache cleared.'));
}


/**
 * Menu Callback
 *
 * Moosend delete list
 *
 */
function moosend_lists_delete($form, &$form_state, $list_id) {
  $delete_list_destination = 'admin/config/services/moosend/lists';
  $list_detail = moosend_get_active_lists_details(1, 10, $list_id);
  $form_state['list'] = array(
    'ID' => $list_detail['ID'],
    'Name' => $list_detail['Name']
    );
  return confirm_form($form,
    t('Reset Moosend mailing lists'),
    $delete_list_destination,
    t('Confirm deleting mailing list "' . $list_detail['Name'] . '"'),
    'Confirm'
  );
}


/**
 * Handler Moosend delete list
 *
 */
function moosend_lists_delete_submit($form, &$form_state) {
  if ( $status = moosend_delete_mailing_list($form_state['list']['ID']) ) {
    drupal_set_message(t('Succefully deleted mailing list "@name"', array( '@name' => $form_state['list']['Name'])));
  }
  else{
    drupal_set_message(t('An error occurred.Please check recent log messages, for further information.'), 'error');
  }
  //Clear cache
  moosend_get_active_lists_details(1, 10, NULL, FALSE);
}


/**
 *  Moosend List form Callback
 *
 * @param   $form
 * @param   &$form_state
 * @return
 */
function moosend_lists_add_lists_form($form, &$form_state) {

    $form['moosend_list_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#description' => t('Create a new mailing list'),
      '#requiered' => TRUE,
    );
    $form['actions']['save'] = array(
      '#type' => 'submit',
      '#value' => t('Add Mailing List'),
    );

    return $form;
}



/**
 * Moosend List validation form Callback
 * @param   $form
 * @param   &$form_state
 * @return
 */
function moosend_lists_add_lists_form_validate($form, &$form_state) {
  if (empty($form_state['values']['moosend_list_name'])) {
    form_set_error('moosend_list_name', t('The list name can not be empty.'));
  }
}



/**
 * Moosend List submit form Callback
 * @param   $form
 * @param   &$form_state
 * @return
 */
function moosend_lists_add_lists_form_submit($form, &$form_state) {
  $list_name = $form_state['values']['moosend_list_name'];
  $new_list_id = moosend_create_mailing_list(check_plain($list_name));
  if (!empty($new_list_id)) {
    moosend_get_active_lists_details(1, 10, NULL, FALSE); // Clear Cache
    drupal_set_message(t('Mailing List "@list" created with ID @id',
      array('@list' => $list_name, '@id' => $new_list_id)),
      'status',
      FALSE);
  }
  $form_state['redirect'][] = 'admin/config/services/moosend/lists';
}



function moosend_lists_add_list_segments($form, &$form_state) {

  $form['#attached']['css'] = array(
    drupal_get_path('module', 'moosend_lists') . '/css/form.css',
  );

  $lists_details = moosend_get_active_lists_details();

  $options = array();
  foreach ($lists_details as $key => $list) {
    $options[$key] = $list['Name'];
  }

  $form['mailing_list'] = array(
    '#type' => 'select',
    '#title' => t('Mailing List'),
    '#description' => t('Select a list to attach your segment.'),
    '#required' => TRUE,
    '#options' => $options,
  );

  $form['segment_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Segment Name'),
    '#description' => t('The Name of the Segement.'),
    '#size' => 35,
    '#maxlength' => 128,
    '#required' => TRUE,
  );

  $form['match_criteria'] = array(
    '#type' => 'select',
    '#title' => t('Match Criteria'),
    '#description' => t('Criteria Pattern to match against.'),
    '#options' => drupal_map_assoc(array('All', 'Any')),
    '#required' => TRUE,
  );

  // Because we have many fields with the same values, we have to set
  // #tree to be able to access them.
  $form['#tree'] = TRUE;

  $form['criteria_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Your Criteria'),
    '#description' => t('Add criteria to specialize your Segment'),
    '#prefix' => '<div id="moosend-criteria-fieldset-wrapper">',
    '#suffix' => '</div>',
  );


  if (empty($form_state['num_criteria'])) {
    $form_state['num_criteria'] = 1;
  }

  for ($i = 0; $i < $form_state['num_criteria']; $i++) {

    $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['starts'] = array(
      '#markup' => '<div class="criteria-wrapper" id="criteria-wrapper-row-' . $i . '">',
      );

    $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_field'][$i] = array(
      '#type' => 'select',
      '#title' => t('Criteria Fields'),
      '#options' => _moosend_criteria_fields(),
      '#ajax' => array(
        'callback' => 'ajax_date_added_criteria_callback',
        'wrapper' => 'criteria-wrapper-row-' . $i ,
      ),
    );

    //if  Date Criteria Field selected
    if (!empty($form_state['values']['criteria_fieldset']['criteria_wrapper-' . $i ]['criteria_field'][$i])) {
      $criteria_field = $form_state['values']['criteria_fieldset']['criteria_wrapper-' . $i ]['criteria_field'][$i];
    }
    else {
      //set default value
      $criteria_field = 'DateAdded';
    }

    //Check for Date Module and set date textfield
    if (module_exists('date_popup')) {
      $element = array(
        '#type' => 'date_popup',
        '#date_format' => 'd/m/Y',
      );
    }
    elseif (module_exists('date')) {
      $element = array(
        '#type' => 'date_select',
        '#date_format' => 'd/m/Y',
      );
    }
    else {
      $element = array(
        '#type' => 'textfield',
        '#size' => 30,
        '#maxlength' => 100,
        '#description' => t('eg. @current_date', array('@current_date' => date('d/m/Y'))),
      );
    }

    if ($criteria_field === 'DateAdded')  {
      $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_comparer'][$i] = array(
        '#type' => 'select',
        '#title' => t('Compare'),
        '#options' => _moosend_criteria_comparer($criteria_field),
      );

    $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_value'][$i] = array('#title' => t('Value')) + $element;

    }
    elseif ($criteria_field === 'CampaignsOpened' || $criteria_field === 'LinksClicked') {

      $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_comparer'][$i] = array(
        '#type' => 'select',
        '#title' => t('Compare'),
        '#options' => _moosend_criteria_comparer($criteria_field),
      );
     $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_value'][$i] = array(
      '#type' => 'textfield',
      '#title' => t('Value'),
      '#size' => 10,
      '#maxlength' => 100,
      );
     $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['date_from'][$i] = array('#title' => t('Date From')) + $element;
     $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['date_to'][$i] = array('#title' => t('Date To')) + $element;

    }
    elseif ($criteria_field === 'LinkURL') {

      $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_comparer'][$i] = array(
        '#type' => 'select',
        '#title' => t('Compare'),
        '#options' => _moosend_criteria_comparer($criteria_field),
      );
      $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_value'][$i] = array(
        '#type' => 'textfield',
        '#title' => t('Value'),
        '#size' => 35,
        '#maxlength' => 128,
      );

    }
    else {

      $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_comparer'][$i] = array(
        '#type' => 'select',
        '#title' => t('Compare'),
        '#options' => _moosend_criteria_comparer(),
      );
      $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['criteria_value'][$i] = array(
        '#type' => 'textfield',
        '#title' => t('Value'),
        '#size' => 35,
        '#maxlength' => 128,
      );
    }

    $form['criteria_fieldset']['criteria_wrapper-' . $i . '']['ends'] = array(
      '#markup' => '</div>',
    );
  }

  $form['criteria_fieldset']['add_criteria'] = array(
    '#type' => 'submit',
    '#value' => t('Add New Criteria'),
    '#submit' => array('ajax_add_more_criteria_add_one'),
    '#ajax' => array(
      'callback' => 'ajax_add_more_criteria_callback',
      'wrapper' => 'moosend-criteria-fieldset-wrapper',
    ),
  );

  if ($form_state['num_criteria'] > 1) {
    $form['criteria_fieldset']['remove_criteria'] = array(
      '#type' => 'submit',
      '#value' => t('Remove one'),
      '#submit' => array('ajax_add_more_criteria_remove_one'),
      '#ajax' => array(
        'callback' => 'ajax_add_more_criteria_callback',
        'wrapper' => 'moosend-criteria-fieldset-wrapper',
      ),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}



/**
 * Callback for "Add Criteria" button.
 *
 * ;Selects and returns the fieldset with the names in it.
 */
function ajax_date_added_criteria_callback($form, &$form_state) {
  $row = substr($form_state['triggering_element']['#name'], 0, -1);
  $row = substr($row, -1);

  return $form['criteria_fieldset']['criteria_wrapper-' . $row . ''];
}



/**
 * Callback for "Add Criteria" button.
 *
 * ;Selects and returns the fieldset with the names in it.
 */
function ajax_add_more_criteria_callback($form, &$form_state) {
  return $form['criteria_fieldset'];
}



/**
 * Submit handler for the "Add Criteria" button.
 *
 * ;Increments the max counter and causes a rebuild.
 */
function ajax_add_more_criteria_add_one($form, &$form_state) {
  $form_state['num_criteria'] ++;
  $form_state['rebuild'] = TRUE;
}



/**
 * Submit handler for the "Remove one" button.
 *
 * ;Decrements the max counter and causes a form rebuild.
 */
function ajax_add_more_criteria_remove_one($form, &$form_state) {
  if ($form_state['num_criteria'] > 1) {
    $form_state['num_criteria'] --;
  }

  $form_state['rebuild'] = TRUE;
}



/**
 * Submit handler for moosend_lists_add_list_segments.
 *
 */
function moosend_lists_add_list_segments_submit($form, &$form_state) {

  $mailing_list_id = $form_state['values']['mailing_list'];
  $segment_name = $form_state['values']['segment_name'];
  $match_criteria = $form_state['values']['match_criteria'];
  $criteria = $form_state['values']['criteria_fieldset'];

  //Create Segment
  $segment_id = moosend_create_segment($mailing_list_id, $segment_name, $match_criteria);

  if (isset($segment_id) && is_numeric($segment_id)) {
    foreach ($criteria as $wrapper => $value) {
      if (strpos($wrapper, 'criteria_wrapper-') !== FALSE) {

        $date_from = NULL;
        $date_to = NULL;
        $criteria_field = reset($value['criteria_field']);
        $comparer = reset($value['criteria_comparer']);
        $criteria_value = reset($value['criteria_value']);

        if (isset($value['date_from']) && isset($value['date_to'])) {
          $date_from = DateTime::createFromFormat('Y-m-d', reset($value['date_from']));
          $date_to = DateTime::createFromFormat('Y-m-d', reset($value['date_to']));
        }

        if (!empty($criteria_value)) {
          // Add Criteria to Segment.
          $response = moosend_add_criteria_to_segment($mailing_list_id, $segment_id, $criteria_field, $comparer, $criteria_value, $date_from, $date_to);
          if (!$response['status']) {
            drupal_set_message(t('An error occurred with Criteria creation.@error_message.'
               , array( '@error_message' => $response['error_message'])),
               'error', FALSE);
            moosend_delete_segment($mailing_list_id, $segment_id);
            return;
          }
        }
      }
    }
  }
  else {
     drupal_set_message(t('An error occurred with Segment creation.Please try again.'), 'error', FALSE);
      return;
  }

  drupal_set_message(t('Segment with name "@name", created succefully for Mailing List ID @mailingList.', array('@name' => $segment_name, '@mailingList' => $mailing_list_id)), 'status', FALSE);

  $form_state['redirect'][] = 'admin/config/services/moosend/lists';
}

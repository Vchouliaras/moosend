<?php


/**
 * [moosend_lists_admin_page description]
 * @return [type] [description]
 */
function moosend_lists_admin_page(){

     $header = array(
        t('Mailing List'),t('Active Subscribers'),t('Unsubscribed'),t('Custom Fields'),t('Operations')
      );

    //Retrieve list from moosend_cache
    $lists = moosend_get_active_lists_details(array(),FALSE);

    if ( empty($lists) ) {
      $message = t('Your mailing lists are empty. You should go to !moosend and create some mailing lists',
          array('!moosend' => l(t('Moosend'),'https://vchouliaras.moosend.com/',
              array(
                'attributes' => array('target' => '_blank'),
                'fragment' => '/mailing-lists',
              )
            )
          )
        );
      drupal_set_message($message,'warning', FALSE);
      //Render array
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
     if ( !empty($list_detail['CustomFieldsDefinition'])) {
        $customFieldsDefinitions = array();
        foreach ($list_detail['CustomFieldsDefinition'] as $key=>$field) {
          $customFieldsDefinitions[] = $field->Name;
        }
        $customFieldsDefinitions = implode(', ',$customFieldsDefinitions);
     }

     //Print table Rows
     $rows[$list_id]['data'] = array(
        $list_detail['Name'],
        $list_detail['ActiveMemberCount'],
        $list_detail['UnsubscribedMemberCount'],
        !empty($customFieldsDefinitions) ? $customFieldsDefinitions : t('None'),
        //Attributes on the cell
        l(t('Edit'),'https://vchouliaras.moosend.com/',
            array(
              'attributes' => array('target' => '_blank'),
              'fragment' => '/mailing-lists/'.$list_id.'/view-list',
            )
          ).' | '.
          l(t('Delete'),'admin/config/services/moosend/lists/delete/'.$list_id,
            array(
              'query' => array('destination' => 'admin/config/services/moosend/lists')
            )
          ),
      );
    }

    $build = array(
        'mailing_list_table' => array(
           '#theme' => 'table',
           '#header' => $header,
           '#rows' => $rows,
          ),
        'refresh_link' => array(
          '#type' => 'markup',
          '#markup' => l(t('Refresh Mailing Lists'),
            'admin/config/services/moosend/lists/clear_cache',
            array('query' => array('destination' => 'admin/config/services/moosend/lists'))
            )
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
function moosend_lists_cache_clear($form, &$form_state) {
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
function moosend_lists_cache_clear_submit($form, &$form_state) {
  moosend_get_active_lists_details(array(),FALSE);
  drupal_set_message(t('Moosend Lists cache cleared.'));
}





/**
 * Menu Callback
 *
 * Moosend delete list
 *
 */
function moosend_lists_delete($form,&$form_state,$list_id) {
  $delete_list_destination = 'admin/config/services/moosend/lists';
  $list_detail = moosend_get_active_lists_details(array($list_id),TRUE);
  $list_detail = reset($list_detail);
  $form_state['list'] = array(
    'Id' => $list_id,
    'Name' => $list_detail['Name']
    );
  return confirm_form($form,
    t('Reset Moosend mailing lists'),
    $delete_list_destination,
    t('Confirm deleting mailing list "'.$list_detail['Name'].'"'),
    'Confirm'
  );
}


/**
 * Handler Moosend delete list
 *
 */
function moosend_lists_delete_submit($form, &$form_state) {
  if ( $status = moosend_delete_mailing_list($form_state['list']['Id']) ) {
    drupal_set_message(t('Succefully deleted mailing list "'.$form_state['list']['Name'].'"'));
  }
  else{
    drupal_set_message(t('An error occurred.Please check Recent log messages, for further information.'));
  }
  //Clear moosend cache
  moosend_get_active_lists_details(array(),FALSE);
}







/**
 * [moosend_lists_add_lists_form description]
 * @param  [type] $form        [description]
 * @param  [type] &$form_state [description]
 * @return [type]              [description]
 */
function moosend_lists_add_lists_form($form,&$form_state){

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
 * [moosend_lists_add_lists_form_validate description]
 * @param  [type] $form        [description]
 * @param  [type] &$form_state [description]
 * @return [type]              [description]
 */
function moosend_lists_add_lists_form_validate($form, &$form_state) {
  if (empty($form_state['values']['moosend_list_name'])) {
    form_set_error('moosend_list_name', t('The list name can not be empty.'));
  }
}

/**
 * [moosend_lists_add_lists_form_submit description]
 * @param  [type] $form        [description]
 * @param  [type] &$form_state [description]
 * @return [type]              [description]
 */
function moosend_lists_add_lists_form_submit($form,&$form_state){
  $list_name = $form_state['values']['moosend_list_name'];
  $new_list_id = moosend_create_mailing_list(check_plain($list_name));
  if (!empty($new_list_id)) {
    moosend_get_active_lists_details(array(),FALSE); // Clear Cache
    drupal_set_message(t('Mailing List "@list" created with ID @id',
      array('@list'=>$list_name,'@id' => $new_list_id)),
      'status',
      FALSE);
  }
  $form_state['redirect'][] = 'admin/config/services/moosend/lists';
}


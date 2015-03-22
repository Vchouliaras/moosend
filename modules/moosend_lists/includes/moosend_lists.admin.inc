<?php


/**
 * [moosend_lists_admin_page description]
 * @return [type] [description]
 */
function moosend_lists_admin_page(){

     $header = array(
        t('Created on'),t('Mailing List'),t('Active Subscribers'),t('Unsubscribed'),t('Actions')
      );

    //Retrieve list from moosend_cache
    $cache = TRUE;
    $list_ids = moosend_get_active_lists(1,10,$cache);
    $lists = moosend_get_active_lists_details($list_ids,$cache);
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
      return array(
            'mailing_list_table' => array(
            '#theme' => 'table',
            '#header' => $header,
          ),
        );
    }

    $rows = array();
    foreach ($lists as $list_id => $list_detail) {
     $rows[$list_id]['data'] = array(
        $list_detail['CreatedOn'],
        $list_detail['Name'],
        $list_detail['ActiveMemberCount'],
        $list_detail['UnsubscribedMemberCount'],
        l(t('View Details'),'https://vchouliaras.moosend.com/',
          array(
            'attributes' => array('target' => '_blank'),
            'fragment' => '/mailing-lists/'.$list_id.'/view-list',
          )
        )
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
 * Moosend List cache clear form.
 *
 */
function moosend_lists_cache_clear($form, &$form_state) {
  $cancel_destination = 'admin/config/services/moosend/lists';
  return confirm_form($form,
    t('Reset Moosend mailing lists'),
    $cancel_destination,
    t('Confirm clearing of Moosend mailing list cache.'),
    'Confirm'
  );
}


/**
 * Handler for lists cache clear form.
 *
 */
function moosend_lists_cache_clear_submit($form, &$form_state) {
  $list_ids = moosend_get_active_lists(1,10,FALSE);
  moosend_get_active_lists_details($list_ids,FALSE);
  drupal_set_message(t('Moosend Lists cache cleared.'));
}




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

function moosend_lists_add_lists_form_validate($form, &$form_state) {
  if (empty($form_state['values']['moosend_list_name'])) {
    form_set_error('moosend_list_name', t('The List name can not be empty.'));
  }
}


function moosend_lists_add_lists_form_submit($form,&$form_state){
  $list_name = $form_state['values']['moosend_list_name'];
  $new_list_id = moosend_create_mailing_list($list_name);
  if (!empty($new_list_id)) {
    drupal_set_message(t('Mailing List "@list" created with ID @id',
      array('@list'=>$list_name,'@id' => $new_list_id)),
      'status',
      FALSE);
  }
  $form_state['redirect'][] = 'admin/config/services/moosend/lists';
}


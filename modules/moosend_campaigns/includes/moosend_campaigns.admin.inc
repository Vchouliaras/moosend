<?php

/**
* @file
*  Moosend Campaigns Module for Admin Interface.
*
*/


/**
 *  Page Callback: Display the Moosend Configuration form
 *
 * @return array
 *
 */
function moosend_campaigns_admin_page() {

    //Get campaigns from moosend_cache
    $campaings = moosend_get_campaings();

    $domain_prefix = variable_get('moosend_user_domain');

    $header = array(
      t('Delivered'), t('Campaing Name'), t('Mailing List/Segment'), t('Status'), t('Reports'), t('Operations')
    );

    if (empty($campaings)) {
      $message = t('You don\'t have any campaigns yet at Moosend. Click the link bellow or go to !moosend and create some.',
        array('!moosend' => l(t('Moosend'), 'https://' . $domain_prefix . '.moosend.com/',
          array(
            'attributes' => array('target' => '_blank'),
            'fragment' => '/campaigns')
          )
        )
      );

      drupal_set_message($message, 'warning', FALSE);

      //Render array
      return $build = array(
          'campaigns_table' => array(
            '#theme' => 'table',
            '#header' => $header,
        ),
      );
    }


    $row = array();
    foreach ($campaings as $campaign_detail) {

     //Get DeliveredOn
     if ( !empty($campaign_detail['DeliveredOn'])) {
      $delivered_on = $campaign_detail['DeliveredOn']->format('d/m/Y H:i');
     }
     else {
      $delivered_on = ' ---------------- ';
     }

     //Get Campaign Name
     $campaing_name = $campaign_detail['Name'];

     //Get Campaign ID
     $campaign_id = $campaign_detail['ID'];

     //Get Mailing List/Segment
     $mailing_list = $campaign_detail['MailingListName'];

     if (!empty($campaign_detail['SegmentName'])) {
        $mailing_list .= '/' . $campaign_detail['SegmentName'];
     }

     //Get Status
     $status = moosend_get_campaign_status_name($campaign_detail['Status']);

    //Print table Rows
    $rows[$campaign_id]['data'] = array(
    $delivered_on,
    $campaing_name,
    $mailing_list,
    $status,
    l(t('Campaign Reports'), 'https://' . $domain_prefix . '.moosend.com/',
        array(
          'attributes' => array('target' => '_blank'),
          'fragment' => '/reports/' . $campaign_id . '/overview',
        )
      ),
    //Attributes on the cell
    l(t('Edit'), 'https://' . $domain_prefix . '.moosend.com/',
        array(
          'attributes' => array('target' => '_blank'),
          'fragment' => '/campaign/' . $campaign_id . '/steps/1',
        )
      ) . ' | ' .
      l(t('Delete'), 'admin/config/services/moosend/campaigns/delete/' . $campaign_id,
        array(
          'query' => array('destination' => 'admin/config/services/moosend/campaigns')
        )
      )
    );
  }

  return $build = array(
    'campaigns_table' => array(
       '#theme' => 'table',
       '#header' => $header,
       '#rows' => $rows,
      ),
    'refresh_link' => array(
      '#type' => 'markup',
      '#markup' => l(t('Refresh Campaigns'),
        'admin/config/services/moosend/campaigns/clear_cache',
        array('query' => array('destination' => 'admin/config/services/moosend/campaigns'))
        )
      )
    );
}


/**
 * [moosend_campaings_add_operations description]
 * @param  [type] $code [description]
 * @return [type]       [description]
 */
function moosend_campaings_add_operations($code) {
  if ($code == CAMPAIGN_DRAFT) {
    $send;
    $scedule;
  }
  else{
    $edit;
    $delete;
  }
}


/**
 * Menu Callback: Moosend Campaigns clear cache form
 *
 */
function moosend_campaigns_clear_cache($form, &$form_state) {
  $cache_clear_destination = 'admin/config/services/moosend/campaigns';
  return confirm_form($form,
    t('Reset Moosend Campaigns'),
    $cache_clear_destination,
    t('Confirm clearing of Moosend Campaigns cache.'),
    'Confirm'
  );
}


/**
 * Handler for Moosend Campaigns clear cache form
 *
 */
function moosend_campaigns_clear_cache_submit($form, &$form_state) {
  moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);
  drupal_set_message(t('Moosend Campaings cache cleared.'));
}


/**
 * Menu Callback:Moosend Delete Campaign
 *
 */
function moosend_campaigns_delete($form, &$form_state, $campaign_id) {
  $delete_campaign_destination = 'admin/config/services/moosend/campaigns';
  $campaings_detail = moosend_get_campaings();
  $form_state['campaign'] = array(
    'ID' => $campaings_detail[$campaign_id]['ID'],
    'Name' => $campaings_detail[$campaign_id]['Name']);

  return confirm_form($form,
    t('Reset Moosend Campaigns'),
    $delete_campaign_destination,
    t('Confirm deleting Campaign "@name"', array('@name' => $campaings_detail[$campaign_id]['Name'])),
    'Confirm'
  );
}


/**
 * Handler Moosend delete Campaign
 *
 */
function moosend_campaigns_delete_submit($form, &$form_state) {
  if ( $status = moosend_delete_campaign($form_state['campaign']['ID']) ) {
    drupal_set_message(t('Successful deleted Campaign "@name"', array('@name' => $form_state['campaign']['Name'])));
  }
  else {
    drupal_set_message(t('An error occurred. Please check your recent log messages, for further information.'), 'error');
  }
  //Clear cache
  moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);
}





/**
 *  Menu Callback: Moosend Add draft Campaing - Wizard form
 *
 */
function moosend_campaigns_add_form($form, &$form_state, $no_js_use = FALSE) {

  // Provide a wrapper around the entire form, since we'll replace the whole
  // thing with each submit.
  $form['#prefix'] = '<div id="campaigns-add-form-wrapper">';
  $form['#suffix'] = '</div>';

  // We want to deal with hierarchical form values.
  $form['#tree'] = TRUE;

  $form['description'] = array(
    '#markup' => '<div>' . t('Follow the step of this form, in order to succefully create a Moosend Campaign'),
  );

  // $form_state['storage'] has no specific drupal meaning, but it is
  // traditional to keep variables for multistep forms there.
  $step = empty($form_state['storage']['step']) ? 1 : $form_state['storage']['step'];
  $form_state['storage']['step'] = $step;

  switch ($step) {
    case 1:

      $form['step1'] = array(
        '#type' => 'fieldset',
        '#title' => t('Step 1: Type and Sender'),
      );
      $form['step1']['Name'] = array(
        '#type' => 'textfield',
        '#title' => t('Campaign Name'),
        '#default_value' => empty($form_state['values']['step1']['Name']) ? '' : $form_state['values']['step1']['Name'],
        '#required' => TRUE,
      );
      $form['step1']['Subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Subject'),
        '#default_value' => empty($form_state['values']['step1']['Subject']) ? '' : $form_state['values']['step1']['Subject'],
        '#required' => TRUE,
      );

      $senders = moosend_get_active_senders();
      foreach ($senders as $sender) {
        $options[$sender['Email']] = $sender['Name'] . ' (' . $sender['Email'] . ')';
      }

      $form['step1']['SenderEmail'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('Sender'),
        '#description' => t('The sender of the campaign.'),
        '#default_value' => empty($form_state['values']['step1']['SenderEmail']) ? '' : $form_state['values']['step1']['SenderEmail'],
        '#required' => TRUE,
      );
      $form['step1']['ReplyToEmail'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('Reply To'),
        '#description' => t('The email address to which recipients replies will arrive.'),
        '#default_value' => empty($form_state['values']['step1']['ReplyToEmail']) ? '' : $form_state['values']['step1']['ReplyToEmail'],
        '#required' => TRUE,
      );

      $site_email = variable_get('site_mail', ini_get('sendmail_from'));
      $options[$site_email] = $site_email;
      $form['step1']['ConfirmationToEmail'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('Confirmation Email'),
        '#description' => t('The email address to which a confirmation message will be sent when the campaign has been successfully sent.'),
        '#default_value' => empty($form_state['values']['step1']['ConfirmationToEmail']) ? '' : $form_state['values']['step1']['ConfirmationToEmail'],
        '#required' => TRUE,
      );
      break;

    case 2:
      $form['step2'] = array(
        '#type' => 'fieldset',
        '#title' => t('Step 2: Select Mailing List'),
      );

      $mailing_lists = moosend_get_active_lists_details();

      $options = array();
      foreach ($mailing_lists as $mailing_list) {
        $options[$mailing_list['ID']] = $mailing_list['Name'];
      }
      $form['step2']['MailingListID'] = array(
        '#type' => 'radios',
        '#options' => $options,
        '#required' => TRUE,
        '#default_value' => empty($form_state['values']['step2']['MailingListID']) ? key($options) : $form_state['values']['step2']['MailingListID'],
      );

      foreach ($mailing_lists as $mailing_list) {
        if ($segments = moosend_get_list_segments($mailing_list['ID'])) {
          $segment_options = array();

          $segment_options['none'] = t('None');
          foreach ($segments as $key => $segment) {
            $segment_options[$segment['id']] = $segment['name'];
          }

          // $form['step2']['SegmentID'][$mailing_list['ID']] = array(
          //   '#type' => 'select',
          //   '#options' => $segment_options,
          //   '#title' => t('Segments for "@mailingList" ',array('@mailingList' => $mailing_list['Name'])),
          //   '#default_value' => empty($form_state['values']['step2']['SegmentID'][$mailing_list['ID']]) ? key($segment_options) : $form_state['values']['step2']['SegmentID'][$mailing_list['ID']],
          //   '#states' => array(
          //     'visible' => array(
          //       ':input[name="step2[MailingListID]"]'=> array('value' => $mailing_list['ID'] )
          //     )
          //   )
          // );
        }
      }
      break;

    case 3:
      $form['step3']['WebLocation'] = array(
        '#type' => 'textfield',
        '#title' => t('Web Location'),
        '#required' => TRUE,
      );
      break;
  }

  if ($step == 3) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t("Create draft Campaign"),
    );
  }

  if ($step < 3) {
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next step'),
      '#ajax' => array(
        'wrapper' => 'campaigns-add-form-wrapper',
        'callback' => 'moosend_campaigns_add_form_callback',
      ),
    );
  }

  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t("Previous step"),
      '#limit_validation_errors' => array(),
      '#submit' => array('moosend_campaigns_add_form_submit'),
      '#ajax' => array(
        'wrapper' => 'campaigns-add-form-wrapper',
        'callback' => 'moosend_campaigns_add_form_callback',
      ),
    );
  }

  return $form;
}


/**
 * Wizard callback function.
 *
 */
function moosend_campaigns_add_form_callback($form, &$form_state) {
  return $form;
}


/**
 * Submit function for moosend_campaigns_add_form.
 *
 */
function moosend_campaigns_add_form_submit($form, &$form_state) {

  $current_step = 'step' . $form_state['storage']['step'];
  if (!empty($form_state['values'][$current_step])) {
    $form_state['storage']['values'][$current_step] = $form_state['values'][$current_step];
  }

  if ($form_state['triggering_element']['#value'] == t('Next step')) {
    $form_state['storage']['step']++;
    $step_name = 'step' . $form_state['storage']['step'];
    if (!empty($form_state['storage']['values'][$step_name])) {
      $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
    }
  }
  if ($form_state['triggering_element']['#value'] == t('Previous step')) {
    $form_state['storage']['step']--;
    $step_name = 'step' . $form_state['storage']['step'];
    $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
  }

  if ($form_state['triggering_element']['#value'] == t('Create draft Campaign')) {
    $params = array();
    foreach ($form_state['storage']['values'] as $key => $value) {
      foreach ($value as $parameter => $name) {
        $params[$parameter] = $name;
        if (strpos($parameter, 'SegmentID') !== FALSE) {
          foreach ($name as  $id => $segment) {
            if ($id == $params['MailingListID']) {
              if ($segment !== 'none') {
                $params[$parameter] = $segment;
              }
              else {
                $params[$parameter] = NULL;
              }
            }
          }
        }
      }
    }

    if (moosend_create_campaign_draft($params)) {
      drupal_set_message(t('Your Moosend Campaign "@name" has been created successfully!', array('@name' => $params['Name'])));

      //clear moosend_cache
      moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);
      return $form_state['redirect'] = 'admin/config/services/moosend/campaigns';

    }
    else {
      drupal_set_message(t('An error occurred creating your Moosend Campaign.Please try again.'), 'error');
    }
  }

  $form_state['rebuild'] = TRUE;
}











































































/**
 *  Menu Callback: Moosend Add draft Campaing - Wizard form
 *
 */
function moosend_campaigns_sent_form($form, &$form_state, $no_js_use = FALSE) {

  // Provide a wrapper around the entire form, since we'll replace the whole
  // thing with each submit.
  $form['#prefix'] = '<div id="campaigns-sent-form-wrapper">';
  $form['#suffix'] = '</div>';

  // We want to deal with hierarchical form values.
  $form['#tree'] = TRUE;


  // $form_state['storage'] has no specific drupal meaning, but it is
  // traditional to keep variables for multistep forms there.
  $step = empty($form_state['storage']['step']) ? 1 : $form_state['storage']['step'];
  $form_state['storage']['step'] = $step;

  switch ($step) {
    case 1:

      $form['step1']['sent_action'] = array(
        '#type' => 'select',
        '#options' => drupal_map_assoc(array('Send Immediatelly', 'Schedule', 'Send a Test')),
        '#title' => t('Specify Action'),
        // '#default_value' => empty($form_state['values']['step1']['SenderEmail']) ? '' : $form_state['values']['step1']['SenderEmail'],
        '#required' => TRUE,
      );
      break;
    case 2:

      $form['step2'] = array(
        '#type' => 'fieldset',
        '#title' => t('Step 2: Select Mailing List'),
      );

      $mailing_lists = moosend_get_active_lists_details();

      $options = array();
      foreach ($mailing_lists as $mailing_list) {
        $options[$mailing_list['ID']] = $mailing_list['Name'];
      }
      $form['step2']['MailingListID'] = array(
        '#type' => 'radios',
        '#options' => $options,
        '#required' => TRUE,
        '#default_value' => empty($form_state['values']['step2']['MailingListID']) ? key($options) : $form_state['values']['step2']['MailingListID'],
      );

      foreach ($mailing_lists as $mailing_list) {
        if ($segments = moosend_get_list_segments($mailing_list['ID'])) {
          $segment_options = array();

          $segment_options['none'] = t('None');
          foreach ($segments as $key => $segment) {
            $segment_options[$segment['id']] = $segment['name'];
          }

          // $form['step2']['SegmentID'][$mailing_list['ID']] = array(
          //   '#type' => 'select',
          //   '#options' => $segment_options,
          //   '#title' => t('Segments for "@mailingList" ',array('@mailingList' => $mailing_list['Name'])),
          //   '#default_value' => empty($form_state['values']['step2']['SegmentID'][$mailing_list['ID']]) ? key($segment_options) : $form_state['values']['step2']['SegmentID'][$mailing_list['ID']],
          //   '#states' => array(
          //     'visible' => array(
          //       ':input[name="step2[MailingListID]"]'=> array('value' => $mailing_list['ID'] )
          //     )
          //   )
          // );
        }
      }
      break;
    case 3:
      $form['step3']['WebLocation'] = array(
        '#type' => 'textfield',
        '#title' => t('Web Location'),
        '#required' => TRUE,
      );
      break;
  }

  if ($step == 3) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t("Create draft Campaign"),
    );
  }

  if ($step < 3) {
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next step'),
      '#ajax' => array(
        'wrapper' => 'campaigns-form-wrapper',
        'callback' => 'moosend_campaigns_add_form_callback',
      ),
    );
  }

  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t("Previous step"),
      '#limit_validation_errors' => array(),
      '#submit' => array('moosend_campaigns_add_form_submit'),
      '#ajax' => array(
        'wrapper' => 'campaigns-form-wrapper',
        'callback' => 'moosend_campaigns_add_form_callback',
      ),
    );
  }

  return $form;
}


























































































































































/**
 * Submit function for moosend_campaigns_add_form.
 *
 */
function moosend_campaigns_sent_form_submit($form, &$form_state) {

  $current_step = 'step' . $form_state['storage']['step'];
  if (!empty($form_state['values'][$current_step])) {
    $form_state['storage']['values'][$current_step] = $form_state['values'][$current_step];
  }

  if ($form_state['triggering_element']['#value'] == t('Next step')) {
    $form_state['storage']['step']++;
    $step_name = 'step' . $form_state['storage']['step'];
    if (!empty($form_state['storage']['values'][$step_name])) {
      $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
    }
  }
  if ($form_state['triggering_element']['#value'] == t('Previous step')) {
    $form_state['storage']['step']--;
    $step_name = 'step' . $form_state['storage']['step'];
    $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
  }

  if ($form_state['triggering_element']['#value'] == t('Create draft Campaign')) {
    $params = array();
    foreach ($form_state['storage']['values'] as $key => $value) {
      foreach ($value as $parameter => $name) {
        $params[$parameter] = $name;
        if (strpos($parameter, 'SegmentID') !== FALSE) {
          foreach ($name as  $id => $segment) {
            if ($id == $params['MailingListID']) {
              if ($segment !== 'none') {
                $params[$parameter] = $segment;
              }
              else {
                $params[$parameter] = NULL;
              }
            }
          }
        }
      }
    }

    if (moosend_create_campaign_draft($params)) {
      drupal_set_message(t('Your Moosend Campaign "@name" has been created successfully!', array('@name' => $params['Name'])));

      //clear moosend_cache
      moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);
      return $form_state['redirect'] = 'admin/config/services/moosend/campaigns';

    }
    else {
      drupal_set_message(t('An error occurred creating your Moosend Campaign.Please try again.'), 'error');
    }
  }

  $form_state['rebuild'] = TRUE;
}

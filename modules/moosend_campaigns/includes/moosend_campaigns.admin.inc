<?php

/**
 * @file
 * Moosend Campaigns Module for Admin Interface.
 */

/**
 * Menu Entity Callback: Moosend Add draft Campaing - Wizard form.
 */
function moosend_campaigns_form($form, &$form_state, $campaign, $op) {

  // Provide a wrapper since we'll replace the whole
  // form with each submit.
  $form['#prefix'] = '<div id="campaigns-add-form-wrapper">';
  $form['#suffix'] = '</div>';

  // We want to deal with hierarchical form values.
  $form['#tree'] = TRUE;

  $form['description'] = array(
    '#markup' => '<div>' . t('Follow the step of this form, in order to succefully create a Moosend Campaign'),
  );

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
        '#default_value' => isset($campaign->name) ? $campaign->name : '',
        '#required' => TRUE,
      );

      $form['step1']['Subject'] = array(
        '#type' => 'textfield',
        '#title' => t('Subject'),
        '#default_value' => isset($campaign->settings['Subject']) ? $campaign->settings['Subject'] : '',
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
        '#default_value' => isset($campaign->settings['SenderEmail']) ? $campaign->settings['SenderEmail'] : key($options),
        '#required' => TRUE,
      );
      $form['step1']['ReplyToEmail'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('Reply To'),
        '#description' => t('The email address to which recipients replies will arrive.'),
        '#default_value' => isset($campaign->settings['ReplyToEmail']) ? $campaign->settings['ReplyToEmail'] : key($options),
        '#required' => TRUE,
      );

      $site_email = variable_get('site_mail', ini_get('sendmail_from'));
      $options[$site_email] = $site_email;
      $form['step1']['ConfirmationToEmail'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => t('Confirmation Email'),
        '#description' => t('The email address to which a confirmation message will be sent when the campaign has been successfully sent.'),
        '#default_value' => isset($campaign->settings['ConfirmationToEmail']) ? $campaign->settings['ConfirmationToEmail'] : key($options),
        '#required' => TRUE,
      );
      break;

    case 2:

      $form['step2'] = array(
        '#type' => 'fieldset',
        '#title' => t('Step 2: Select Mailing List'),
      );

      $mailing_lists = moosend_get_active_lists_details();
      $mailing_lists_options = array();
      foreach ($mailing_lists as $mailing_list) {
        $mailing_lists_options[$mailing_list['ID']] = $mailing_list['Name'];
      }

      $form['step2']['MailingListID'] = array(
        '#type' => 'select',
        '#options' => $mailing_lists_options,
        '#required' => TRUE,
        '#default_value' => isset($campaign->settings['MailingListID']) ? $campaign->settings['MailingListID'] : key($mailing_lists_options),
        '#ajax' => array(
          'callback' => '_moosend_campaigns_add_form_segments_callback',
          'wrapper' => 'segments-fields-wrapper',
          'progress' => array(
            'type' => 'throbber',
            'message' => t('Getting Mailing Lists Segments...'),
          ),
        ),
      );

      $form['step2']['segments_group'] = array(
        '#prefix' => '<div id="segments-fields-wrapper">',
        '#suffix' => '</div>',
        '#type' => 'fieldset',
        '#title' => t('Segments'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      if (!empty($campaign->settings['MailingListID'])) {
        $choosen_mailing_list = $campaign->settings['MailingListID'];
      }
      elseif (!empty($form_state['values']['step2']['MailingListID'])) {
        $choosen_mailing_list = $form_state['values']['step2']['MailingListID'];
      }
      else {
        $choosen_mailing_list = key($mailing_lists_options);
      }

      if ($segments = moosend_get_list_segments($choosen_mailing_list)) {
        $segment_options = array();
        $segment_options['none'] = t('None');
        foreach ($segments as $segment) {
          $segment_options[$segment['id']] = $segment['name'];
        }
        $form['step2']['segments_group']['SegmentID'] = array(
          '#type' => 'select',
          '#options' => $segment_options,
        );
        if (isset($campaign->settings['SegmentID'])) {
          $form['step2']['segments_group']['SegmentID']['#default_value'] = $campaign->settings['SegmentID'];
        }
      }

      break;

    case 3:
      $form['step3']['WebLocation'] = array(
        '#type' => 'textfield',
        '#title' => t('Web Location'),
        '#required' => TRUE,
        '#default_value' => isset($campaign->settings['WebLocation']) ? $campaign->settings['WebLocation'] : '',
      );
      break;
  }

  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => t("Previous step"),
      '#limit_validation_errors' => array(),
      '#submit' => array('moosend_campaigns_form_submit'),
      '#ajax' => array(
        'wrapper' => 'campaigns-add-form-wrapper',
        'callback' => '_moosend_campaigns_add_form_callback',
      ),
    );
  }

  if ($step < 3) {
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next step'),
      '#ajax' => array(
        'wrapper' => 'campaigns-add-form-wrapper',
        'callback' => '_moosend_campaigns_add_form_callback',
      ),
    );
  }

  if ($step == 3) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => isset($campaign->is_new) ? t("Create draft Campaign") : t('Update Campaign'),
    );
  }

  return $form;
}


/**
 * Wizard callback function for Mailing Lists segments.
 */
function _moosend_campaigns_add_form_segments_callback($form, $form_state) {
  return $form['step2']['segments_group'];
}


/**
 * Wizard callback function for steps.
 */
function _moosend_campaigns_add_form_callback($form, $form_state) {
  return $form;
}


/**
 * Return default values for multistep form.
 */
function _moosend_campaings_get_default_value($campaign_field, $form_state_field) {
  //@TODO   Set defaults values to form either from Entity of form_state values
}


/**
 * Submit handler for moosend_campaigns_form().
 */
function moosend_campaigns_form_submit($form, &$form_state) {

  $current_step = 'step' . $form_state['storage']['step'];
  if (!empty($form_state['values'][$current_step])) {
    $form_state['storage']['values'][$current_step] = $form_state['values'][$current_step];
  }

  // Normalize multistep values.
  foreach ($form_state['storage']['values'] as $key => $multistep_values) {
    foreach ($multistep_values as $key => $value) {
      if (is_array($value)) {
        $params[key($value)] = (reset($value) == 'none') ? NULL : reset($value);
      }
      else {
        $params[$key] = $value;
      }
    }
  }

  switch ($form_state['triggering_element']['#value']) {

    case t('Next step'):
      $form_state['storage']['step']++;
      $step_name = 'step' . $form_state['storage']['step'];
      if (!empty($form_state['storage']['values'][$step_name])) {
        $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
      }
      break;

    case t('Previous step'):
      $form_state['storage']['step']--;
      $step_name = 'step' . $form_state['storage']['step'];
      $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
      break;

    case t('Create draft Campaign'):
      // First try to create Campaing at Moosend.

      if ($campaign_id = moosend_create_campaign_draft($params)) {

        // Clear moosend_cache.
        moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);

        // Get Entity.
        $campaign = !isset($form_state['moosend_campaigns']) ?
        entity_get_controller('moosend_campaigns')->create() : $form_state['moosend_campaigns'];

        // Populate Entity.
        $campaign->campaign_id = $campaign_id;
        $campaign->status = MOOSEND_CAMPAIGNS_CAMPAIGN_DRAFT;
        $campaign->name = $params['Name'];
        $campaign->settings['Subject'] = $params['Subject'];
        $campaign->settings['SenderEmail'] = $params['SenderEmail'];
        $campaign->settings['ReplyToEmail'] = $params['ReplyToEmail'];
        $campaign->settings['ConfirmationToEmail'] = $params['ConfirmationToEmail'];
        $campaign->settings['MailingListID'] = $params['MailingListID'];
        $campaign->settings['SegmentID'] = $params['SegmentID'];
        $campaign->settings['WebLocation'] = $params['WebLocation'];

        if ($campaign->save()) {
          drupal_set_message(t('Your Moosend Campaign "@name" has been created successfully!',
            array('@name' => $params['Name'])));
        }
      }
      else {
        drupal_set_message(t('Unable to create draft Campaign. Check Reports for more info.'), 'error');
      }

      $form_state['redirect'] = 'admin/config/services/moosend/campaigns';
      return $form_state['redirect'];

    break;

    case t('Update Campaign'):
      $campaign_id = $form_state['moosend_campaigns']->campaign_id;

      if (moosend_update_MOOSEND_CAMPAIGNS_CAMPAIGN_DRAFT($campaign_id, $params)) {

        // Clear moosend_cache.
        moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);

        // Get Entity.
        $campaign = $form_state['moosend_campaigns'];

        // Populate Entity.
        $campaign->campaign_id = $campaign_id;
        $campaign->status = MOOSEND_CAMPAIGNS_CAMPAIGN_DRAFT;
        $campaign->name = $params['Name'];
        $campaign->settings['Subject'] = $params['Subject'];
        $campaign->settings['SenderEmail'] = $params['SenderEmail'];
        $campaign->settings['ReplyToEmail'] = $params['ReplyToEmail'];
        $campaign->settings['ConfirmationToEmail'] = $params['ConfirmationToEmail'];
        $campaign->settings['MailingListID'] = $params['MailingListID'];
        $campaign->settings['SegmentID'] = $params['SegmentID'];
        $campaign->settings['WebLocation'] = $params['WebLocation'];

        if ($campaign->save()) {
          drupal_set_message(t('Your Moosend Campaign "@name" has been updated successfully!',
            array('@name' => $params['Name'])));
        }
      }
      else {
        drupal_set_message(t('Unable to update draft Campaign. Check your reports for more info.'), 'error');
      }

      $form_state['redirect'] = 'admin/config/services/moosend/campaigns';
      return $form_state['redirect'];

    break;
  }

  $form_state['rebuild'] = TRUE;
}

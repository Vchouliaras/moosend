<?php

/**
 * @file
 * Moosend module admin settings.
 */


/**
 * Return the MailChimp global settings form.
 *
 */
function moosend_admin_configuration() {
  $form = array();

  $form['moosend_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Moosend API Key'),
    '#required' => TRUE,
    '#default_value' => variable_get('moosend_api_key'),
    '#description' => t('The API key for your Moosend account. Get a valid API key at !moosend.',
                    array('!moosend' => l(t('Moosend'), 'https://moosend.com/'))),
  );

  $form['moosend_user_domain'] = array(
    '#type' => 'textfield',
    '#title' => t('Moosend Domain prefix'),
    '#required' => TRUE,
    '#default_value' => variable_get('moosend_user_domain'),
    '#description' => t('The Domain prefix of your Moosend account, e.g. mydomain.moosend.com.'),
  );

  $form['actions'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
  );

  return $form;
}

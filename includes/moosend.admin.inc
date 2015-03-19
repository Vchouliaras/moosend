<?php

/**
 * @file
 * Moosend module admin settings.
 */



/**
 * Return the MailChimp global settings form.
 *
 */
function moosend_admin_settings() {

  $form = array();
  $form['moosend_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Moosend API Key'),
    '#required' => TRUE,
    '#default_value' => variable_get('moosend_api_key'),
    '#description' => t('The API key for your Moosend account. Get a valid API key at !moosend.',
                    array('!moosend' => l(t('Moosend '), 'https://moosend.com/'))),
  );

  return system_settings_form($form);
}



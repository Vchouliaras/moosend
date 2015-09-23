<?php

/**
 * @file
 * Moosend module admin Configuration Settings.
 */


/**
 * Return the Moosend configuration form.
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


/**
 *  Submit Handler
 *
 */
function moosend_admin_configuration_submit($form, &$form_state) {
  $moosend_api_key = $form_state['values']['moosend_api_key'];
  $moosend_user_domain = $form_state['values']['moosend_user_domain'];
  variable_set('moosend_api_key', $moosend_api_key);
  variable_set('moosend_user_domain', $moosend_user_domain);

  if (!initialize_moosend_api())  {
    drupal_set_message(t('Could not initialize connection with Moosend API. Please check your configuration settings.'), 'error');
    variable_del('moosend_api_key');
    variable_del('moosend_user_domain');
  }

  cache_clear_all('*', 'cache_moosend', TRUE);
}

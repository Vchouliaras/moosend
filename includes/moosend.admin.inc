<?php

/**
 * @file
 * Moosend module admin Configuration Settings.
 */


/**
 * Return the Moosend configuration form.
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

  $form['moosend_cron'] = array(
    '#type' => 'checkbox',
    '#title' => 'Use Drupal\'s cron Queue API system.',
    '#description' => t('Leverage Drupal\'s Queue Api to operate on a large amounts of Moosend operations (Subscribe, Unsuscribe, Remove).'),
    '#default_value' => variable_get('moosend_cron', FALSE),
  );

  $form['actions'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
  );

  return $form;
}


/**
 * Submit Handler
 */
function moosend_admin_configuration_submit($form, &$form_state) {
  $moosend_api_key = $form_state['values']['moosend_api_key'];
  $moosend_user_domain = $form_state['values']['moosend_user_domain'];
  $moosend_cron = $form_state['values']['moosend_cron'];

  variable_set('moosend_api_key', $moosend_api_key);
  variable_set('moosend_user_domain', $moosend_user_domain);
  ($moosend_cron) ? variable_set('moosend_cron', TRUE) : variable_set('moosend_cron', FALSE);

  if (!moosend_initialize_api()) {
    drupal_set_message(t('Could not initialize connection with Moosend API. Please check your configuration settings.'), 'error');
    variable_del('moosend_api_key');
    variable_del('moosend_user_domain');
    variable_del('moosend_cron');
  }

  cache_clear_all('*', 'cache_moosend', TRUE);
}

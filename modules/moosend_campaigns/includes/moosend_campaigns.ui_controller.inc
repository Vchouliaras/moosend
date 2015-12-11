<?php

/**
 * @file
 *
 * Provides a controller for customizing Moosend Campaigns Admin UI
 */

class MoosendCampaignsUIController extends EntityDefaultUIController{


  public $overviewPagerLimit = MOOSEND_PAGER;

  /**
   * Overrides parent::hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_object';

    $items[$this->path]['title'] = 'Campaigns';
    $items[$this->path]['description'] = 'Manage Moosend Campaigns';
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = 0;

    $items[$this->path . '/add']['title callback'] = 'moosend_entity_ui_title_callback';

    $items[$this->path . '/' . $wildcard . '/send'] = array(
      'title' => 'Send the Campaign',
      'page callback' => 'drupal_get_form',
      'page arguments' => array($this->entityType . '_operation_form', $this->entityType, $this->id_count, 'send'),
      'load arguments' => array($this->entityType),
      'access callback' => 'entity_access',
      'access arguments' => array('create', $this->entityType),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
    );

    //Removes predefine clone operation
    unset($items[$this->path . '/manage/' . $wildcard . '/clone']);

    $items[$this->path . '/' . $wildcard . '/clone'] = array(
      'title' => 'Clone the Campaign',
      'page callback' => 'drupal_get_form',
      'page arguments' => array($this->entityType . '_operation_form', $this->entityType, $this->id_count, 'clone'),
      'load arguments' => array($this->entityType),
      'access callback' => 'entity_access',
      'access arguments' => array('create', $this->entityType),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
    );

    return $items;
  }


  /**
   * Generates the table headers for the overview table.
   *
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {
    $header = array();
    $append_header[]  = t('Name');
    $append_header[]  = array(
      'data' => t('Created at'),
      'type' => 'property',
      'specifier' => 'created_at', '
      sort' => 'desc'
      );

    $append_header[]  = t('Delivered On');
    $append_header[]  = t('Status');
    $append_header[]  = t('Reports');
    $header = array_merge($header, $append_header);

    // Add operations with the right colspan.
    $header[] = array('data' => t('Operations'), 'colspan' => $this->operationCount());

    return $header;
  }


/**
  * Returns the operation count for calculating colspans.
  */
  protected function operationCount() {
      $count = parent::operationCount();
      return $count + 1;
  }


  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   *
   * @param $additional_cols
   *   Additional columns to be added after the entity label column.
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {
    $domain_prefix = variable_get('moosend_user_domain');

    $additional_cols[] = format_date($entity->created_at, 'medium');
    $additional_cols[] = !empty($entity->settings['DeliveredOn']) ?
      format_date($entity->settings['DeliveredOn'], 'medium') : '----------------------';
    $additional_cols[] = moosend_get_campaign_status_name($entity->status);
    $additional_cols[] = l(t('Campaign Reports'), 'https://' . $domain_prefix . '.moosend.com/', array(
          'attributes' => array('target' => '_blank'),
          'fragment' => '/reports/' . $id . '/overview',
        )
      );

    $row =  parent::overviewTableRow($conditions, $id, $entity, $additional_cols);

    $row[] = l(t('clone'), $this->path . '/manage/' . $id . '/clone');

    //Campaign successful Status responses when campaign is send.
    $statuses = array(
      CAMPAIGN_READY_TO_SEND,
      CAMPAIGN_SEND,
      CAMPAIGN_SMTP_READY_TO_SEND,
      CAMPAIGN_SENDING);

    if(in_array($entity->status, $statuses)) {
      $row[] = t('<strike>sent</strike>');
    }
    else {
      $row[] = l(t('send'), $this->path . '/manage/' . $id . '/send');
    }

    return $row;
  }


  /**
   * [MoosendCampaignsOp description]
   *
   */
  public function moosendCampaignOp(){
    return array(
      'send_a_test' => t('Send a Test'),
      'send_immediatelly' => t('Send Immediatelly'),
      // 'schedule' => t('Schedule')
    );
  }


  /**
   * [moosendCampaignTimezones description]
   * @return [type] [description]
   */
  public function moosendCampaignTimezones(){
    return drupal_map_assoc(array('Dateline Standard Time','Samoa Standard Time','Hawaiian Standard Time','Alaskan Standard Time','Pacific Standard Time','Pacific Standard Time (Mexico)','US Mountain Standard Time','Mountain Standard Time (Mexico)','Mountain Standard Time','Central Standard Time','Central Standard Time (Mexico)','Canada Central Standard Time','SA Pacific Standard Time','US Eastern Standard Time','Eastern Standard Time','Venezuela Standard Time','Atlantic Standard Time','SA Western Standard Time','Central Brazilian Standard Time','Pacific SA Standard Time','Newfoundland Standard Time','E. South America Standard Time','Argentina Standard Time','SA Eastern Standard Time','Greenland Standard Time','Montevideo Standard Time','Mid-Atlantic Standard Time','Azores Standard Time','Cape Verde Standard Time','Greenwich Standard Time','GMT Standard Time','Morocco Standard Time','W. Central Africa Standard Time','Central European Standard Time','Romance Standard Time','W. Europe Standard Time','Namibia Standard Time','E. Europe Standard Time','Israel Standard Time','FLE Standard Time','South Africa Standard Time','Egypt Standard Time','Middle East Standard Time','GTB Standard Time','Jordan Standard Time','Iran Standard Time','Georgian Standard Time','E. Africa Standard Time','Russian Standard Time','Arab Standard Time','Arabic Standard Time','Caucasus Standard Time','Mauritius Standard Time','Azerbaijan Standard Time','Arabian Standard Time','Afghanistan Standard Time','West Asia Standard Time','Pakistan Standard Time','Ekaterinburg Standard Time','Sri Lanka Standard Time','India Standard Time','Nepal Standard Time','N. Central Asia Standard Time','Central Asia Standard Time','Myanmar Standard Time','North Asia Standard Time','SE Asia Standard Time','Taipei Standard Time','W. Australia Standard Time','Singapore Standard Time','North Asia East Standard Time','China Standard Time','Yakutsk Standard Time','Korea Standard Time','Tokyo Standard Time','AUS Central Standard Time','Cen. Australia Standard Time','AUS Eastern Standard Time','West Pacific Standard Time','Tasmania Standard Time','Vladivostok Standard Time','Central Pacific Standard Time','New Zealand Standard Time','Tonga Standard Time')
      );
  }



  /**
   * Builds the operation form.
   *
   * For the export operation a serialized string of the entity is directly
   * shown in the form (no submit function needed).
   */
  public function operationForm($form, &$form_state, $entity, $op) {
    $form['#tree'] = TRUE;

    switch ($op) {
      case 'send':
        return $this->operationFormSend($form, $form_state, $entity);
      case 'clone':
        return $this->operationFormClone($form, $form_state, $entity);
        break;
    }

    return parent::operationForm($form, $form_state, $entity, $op);
  }



/**
 * [operationFormSend description]
 * @param  [type] $form        [description]
 * @param  [type] &$form_state [description]
 * @param  [type] $entity      [description]
 * @return [type]              [description]
 */
protected function operationFormSend($form, &$form_state, $entity){

  $form['send_action'] = array(
    '#type' => 'select',
    '#options' => $this->moosendCampaignOp(),
    '#default_value' => array('Send a Test'),
    '#title' => t('Specify Action'),
    '#required' => TRUE,
    '#ajax' => array(
      'wrapper' => 'send-campaign-wrapper',
      'callback' => '_moosend_campaign_ajax_callback'
      )
  );

  $send_action = isset($form_state['values']['send_action']) ?
    $form_state['values']['send_action'] : 'send_a_test';

  $form['wrapper']['starts'] = array(
    '#markup' => '<div id="send-campaign-wrapper">'
  );

  if($send_action == 'send_a_test') {
    $form['wrapper']['test']['senders_email'] = array(
      '#type' => 'textarea',
      '#cols' => 20,
      '#rows' => 5,
      '#resizable' => TRUE,
      '#default_value' => variable_get('site_mail', ini_get('sendmail_from')),
      '#title' => t('Specify Email Senders'),
      '#description' => t('Multiple emails, must be comma seperated'),
      '#required' => TRUE,
    );
  }
  elseif ($send_action == 'schedule') {
    $campaigns = moosend_get_campaings();

    $form['wrapper']['schedule']['moosend_timezone'] = array(
      '#type' => 'select',
      '#title' => t('Campaign Timezone'),
      '#description' => t('The preferred Timezone to schedule the Campaign'),
      '#options' => $this->moosendCampaignTimezones(),
      '#default_value' => $campaigns[$entity->campaign_id]['ScheduledForTimezone'],
      '#required' => TRUE,
    );

    //Check for Date Module and set date textfield
    $date_element_defaults = array(
      '#title' => t('Set Campaign Schedule Time'),
      '#required' => TRUE,
    );

    if (module_exists('date_popup')) {
      $element = array(
        '#type' => 'date_popup',
        '#date_format' => 'd/m/Y H:i',
      ) + $date_element_defaults;
    }
    elseif (module_exists('date')) {
      $element = array(
        '#type' => 'date_select',
        '#date_format' => 'd/m/Y H:i',
      ) + $date_element_defaults;
    }
    else {
      $element = array(
        '#type' => 'textfield',
        '#size' => 30,
        '#maxlength' => 100,
        '#description' => t('eg. @current_date', array('@current_date' => date('d/m/Y H:i'))),
      ) + $date_element_defaults;
    }

    $form['wrapper']['schedule']['moosend_datetime'] = $element;
  }

  $form['wrapper']['ends'] = array(
    '#markup' => '</div>'
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send Campaign'),
  );

  return $form;
}


protected function operationFormClone($form, &$form_state, $entity){
 $label = entity_label($this->entityType, $entity);
 $confirm_question = t('Are you sure you want to clone the campaign %label?',
    array('%entity' => $this->entityInfo['label'], '%label' => $label));

  return confirm_form($form, $confirm_question, $this->path);
}


  /**
   * Operation form validation callback.
   *
   */
  public function operationFormValidate($form, &$form_state) {
    parent::operationFormValidate($form, $form_state);
  }



  /**
   * Applies an operation to the given entity.
   *
   * Note: the export operation is directly carried out by the operationForm()
   * method.
   *
   * @param string $op
   *   The operation (revert, delete or import).
   * @param $entity
   *   The entity to manipulate.
   *
   * @return
   *   The status message of what has been applied.
   */
  public function applyOperation($op, $entity) {
    parent::applyOperation($op, $entity);

    //To avoid Strict warning: Declaration, use this
    $args = func_get_args();
    $values = $args[2];

    switch ($op) {
      case 'send':
        if ($values['send_action'] == 'send_a_test') {
          $senders = explode(',', $values['wrapper']['test']['senders_email']);

          if(moosend_send_test_campaign($entity->campaign_id, $senders)) {
            return t('Test Campaign "@name" was sent successfully',
              array('@name' => $entity->name));
          }
        }
        elseif ($values['send_action'] == 'send_immediatelly') {
          if(moosend_send_campaign($entity->campaign_id)) {

          //Clear campaigns cache
          $campaigns = moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);

          $entity->status = $campaigns[$entity->campaign_id]['Status'];
          entity_save($this->entityType, $entity);

           return t('Campaign "@name" was sent. Status: @status',
              array('@name' => $entity->name, '@status' => moosend_get_campaign_status_name($entity->status)));
          }
        }
        elseif ($values['send_action'] == 'schedule') {
          $timezone = $values['wrapper']['schedule']['moosend_timezone'];
          $datetime = $values['wrapper']['schedule']['moosend_datetime'];

          //@TODO Support for Scheduling a Campaign in PHP Wrapper
        }
        case 'clone':
          $clone = moosend_campaign_clone($entity->campaign_id);

          //Clear campaigns cache
          $campaigns = moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);

          if(!empty($clone)) {

             $entity = clone $entity;
             $entity->campaign_id = $clone->getID();
             $entity->is_new = TRUE;
             $entity->status = CAMPAIGN_DRAFT;

             entity_save($this->entityType, $entity);

             return t('Campaign "@name" was cloned successfully',
              array('@name' => $entity->name));
          }
      }

  }



 /**
   * Operation form submit callback.
   *
   * Override in order to provide $form_state values.
   */
  public function operationFormSubmit($form, &$form_state) {
    $msg = $this->applyOperation($form_state['op'], $form_state[$this->entityType], $form_state['values']);
    drupal_set_message($msg);
    $form_state['redirect'] = $this->path;
  }


  /**
   * Overrides EntityAPIController::buildQuery().
   *
   * Implement to order by created date since our IDs are not numeric.
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    $query->orderBy('created_at', 'DESC');
    return $query;
  }
}


  /**
   *  Ajax Callback for "Specify Action"
   *
   */
  function _moosend_campaign_ajax_callback($form, $form_state) {
    return $form['wrapper'];
  }

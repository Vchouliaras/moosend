<?php

/**
 * @file
 *
 * Provides a controller for customizing Moosend Campaigns Admin UI
 */

class MoosendCampaignsUIController extends EntityDefaultUIController{

    //Override pager
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

    $items[$this->path . '/manage/' . $wildcard . '/send'] = array(
      'title' => t('Send Campaign'),
      'page callback' => 'drupal_get_form',
      'page arguments' => array($this->entityType . '_send_campaign_form', $this->id_count + 1),
      'load arguments' => array($this->entityType),
      'description' => 'Send a ' . $plural_label . '.',
      'access callback' => 'entity_access',
      'access arguments' => array('view', $this->entityType),
      // 'file' => 'includes/moosend_campaigns.admin.inc',
    );

    return $items;
  }



  /**
   * Provides definitions for implementing hook_forms().
   *
   * Use per bundle form ids if possible, such that easy per bundle alterations
   * are supported too.
   *
   * Note that for performance reasons, this method is only invoked for forms,
   * which receive the entity_type as first argument. Thus any forms added, must
   * follow that pattern.
   *
   * @see entity_forms()
   */
  // public function hook_forms() {

  //   $forms = parent::hook_forms();

  //   // The overview and the operation form are implemented by the controller,
  //   // the callback and validation + submit handlers just invoke the controller.
  //   $forms[$this->entityType . '_send_campaign_form'] = array(
  //     'callback' => 'entity_ui_send_campaign_form',
  //     'wrapper_callback' => 'entity_ui_form_defaults',
  //   );

  //   return $forms;
  // }


  /**
   * Overview form validation callback.
   *
   * @param $form
   *   The form array of the overview form.
   * @param $form_state
   *   The overview form state which will be used for validating.
   */
  // public function send_campaignFormValidate($form, &$form_state) {
  //   dsm('validate');
  // }

  /**
   * Overview form submit callback.
   *
   * @param $form
   *   The form array of the overview form.
   * @param $form_state
   *   The overview form state which will be used for submitting.
   */
  // public function send_campaignFormSubmit($form, &$form_state) {
  //   dsm('submit');
  // }


// public function campaignSendForm($form, $form_state){
//   dsm(func_get_args());
// }



    /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {

    $header = array();
    $append_header[]  = t('Name');
    $append_header[]  = array('data' => t('Created at'), 'type' => 'property', 'specifier' => 'created_at', 'sort' => 'desc');
    $append_header[]  = t('Status');
    $append_header[]  = t('Reports');
    $header = array_merge($header, $append_header);

    // Add operations with the right colspan.
    $header[] = array('data' => t('Operations'), 'colspan' => $this->operationCount());

    return $header;
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

    $additional_cols[] = format_date($entity->created_at, 'custom', 'j M Y, H:G');
    $additional_cols[] = moosend_get_campaign_status_name($entity->status);
    $additional_cols[] = l(t('Campaign Reports'), 'https://' . $domain_prefix . '.moosend.com/', array(
          'attributes' => array('target' => '_blank'),
          'fragment' => '/reports/' . $id . '/overview',
        )
      );

    return parent::overviewTableRow($conditions, $id, $entity, $additional_cols);
  }
}




function moosend_campaigns_send_campaign_form(){
  dsm(func_get_args(oid));
}




/**
 * Form builder function for the overview form.
 *
 * @see EntityDefaultUIController::overviewForm()
 */
function entity_ui_send_campaign_form($form, &$form_state, $entity_type) {
  return entity_ui_controller($entity_type)->campaignSendForm($form, $form_state);
}

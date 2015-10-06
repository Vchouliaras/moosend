<?php

/**
 * @file
 * Provides a controller for building an Subscription Entity overview form.
 */

class MoosendSubscriptionsUIController extends EntityDefaultUIController{

  //Override pager
  public $overviewPagerLimit = MOOSEND_PAGER;

    /**
   * Overrides parent::hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();

    $items[$this->path]['title'] = 'Subscription Forms';
    $items[$this->path]['description'] = 'Manage Moosend Subscription forms';
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['weight'] = 0;
    $items[$this->path]['access callback'] = 'moosend_subscriptions_access_callback';

    return $items;
  }

    /**
   * Generates the table headers for the overview table.
   */
  protected function overviewTableHeaders($conditions, $rows, $additional_header = array()) {

    $header = array();
    $append_header[]  = t('Name');
    $append_header[]  = array('data' => t('Created at'), 'type' => 'property', 'specifier' => 'created_at', 'sort' => 'desc');
    $append_header[]  = t('Mailling List');
    $append_header[]  = t('Display Mode');
    $append_header[]  = t('Custom Form Fields');
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

    $created_at = format_date($entity->created_at, 'custom', 'j M Y, H:G');
    $mailing_list = moosend_get_active_lists_details(1, 10 , $entity->list);
    $domain_prefix = variable_get('moosend_user_domain');

    $mailing_list = l($mailing_list['Name'], 'https://' . $domain_prefix . '.moosend.com/',
      array(
        'attributes' => array('target' => '_blank'),
        'fragment' => '/mailing-lists/' . $entity->list . '/view-list',
      )
    );
    $mode = NULL;
    switch ($entity->mode) {
      case MOOSEND_SUBSCRIPTIONS_BLOCK_DISPLAY:
        $mode = l(t('Block'), 'admin/structure/block');
        break;
      case MOOSEND_SUBSCRIPTIONS_PAGE_DISPLAY:
        $mode =  l(t('Page'), $entity->settings['page_url']);
        break;
      case MOOSEND_SUBSCRIPTIONS_BOTH_PAGE_BLOCK:
        $mode = l(t('Block'), 'admin/structure/block') . ' & ' . l(t('Page'), $entity->settings['page_url']);
        break;
    }

    if (isset($entity->settings['custom_fields'])) {
        $customFields = array();
        foreach ($entity->settings['custom_fields'] as $key => $field) {
          $customFields[] = $field->Name;
        }
        $customFields = !empty($customFields) ? implode(', ', $customFields) : t('None');
     }

    $additional_cols[] = $created_at;
    $additional_cols[] = $mailing_list;
    $additional_cols[] = $mode;
    $additional_cols[] = $customFields;

    return parent::overviewTableRow($conditions, $id, $entity, $additional_cols);
  }
}

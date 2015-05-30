<?php


class MoosendSubscriptionsUIController extends EntityDefaultUIController{

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
    $header = array_merge($header,$append_header);

    // Add operations with the right colspan.
    $header[] = array('data' => t('Operations'), 'colspan' => $this->operationCount());

    return $header;
  }



  /**
   * Overrides parent::overviewTable().
   */
  // public function overviewTable($conditions = array()) {
  //   $render = parent::overviewTable($conditions);

  //   kpr($render);
  //   foreach ($variable as $key => $row) {
  //     $subscription =  $row[0]['data']['options']['entity'];
  //   }
  //   $mc_lists = mailchimp_get_lists();
  //   foreach ($render['#rows'] as &$row) {
  //     $signup = $row[0]['data']['#url']['options']['entity'];
  //     $modes = NULL;
  //     $block_only = FALSE;
  //     switch ($signup->mode) {
  //       case MAILCHIMP_SIGNUP_BLOCK:
  //         $modes = l(t('Block'), 'admin/structure/block');
  //         $block_only = TRUE;
  //         break;

  //       case MAILCHIMP_SIGNUP_PAGE:
  //         $modes = l(t('Page'), $signup->settings['path']);
  //         break;

  //       case MAILCHIMP_SIGNUP_BOTH:
  //         $modes = l(t('Block'), 'admin/structure/block') . ' and ' . l(t('Page'), $signup->settings['path']);
  //         break;

  //     }
  //     $list_labels = array();
  //     foreach ($signup->mc_lists as $list_id) {
  //       $list_labels[] = l($mc_lists[$list_id]['name'], 'https://admin.mailchimp.com/lists/dashboard/overview?id=' . $mc_lists[$list_id]['web_id']);
  //     }
  //     if ($block_only) {
  //       $access = 'N/A - this form only exists as a block';
  //     }
  //     else {
  //       $all_roles_allowed = user_roles(FALSE, 'mailchimp_signup_all_forms' . $signup->name);
  //       $page_roles_allowed = user_roles(FALSE, 'mailchimp_signup_form_' . $signup->name);
  //       $roles_allowed = array_merge($all_roles_allowed, $page_roles_allowed);
  //       $access = implode(', ', $roles_allowed);
  //       $actions[] = l(t('Permissions'), 'admin/people/permissions', array('fragment' => 'edit-mailchimp-signup-all-forms'));
  //     }
  //     $new_row = array();
  //     // Put the label column data first:
  //     $new_row[] = array_shift($row);
  //     // Now our custom columns:
  //     $new_row[] = $modes;
  //     $new_row[] = implode(', ', $list_labels);
  //     $new_row[] = $access;
  //     // Now tack on the remaining built-in rows:
  //     $row = array_merge($new_row, $row);
  //   }
  //   $new_header[] = array_shift($render['#header']);
  //   $new_header[] = t('Display Mode(s)');
  //   $new_header[] = t('MailChimp Lists');
  //   $new_header[] = t('Page Access');
  //   $render['#header'] = array_merge($new_header, $render['#header']);
  //   return $render;
  // }




  /**
   * Generates the row for the passed entity and may be overridden in order to
   * customize the rows.
   *
   * @param $additional_cols
   *   Additional columns to be added after the entity label column.
   */
  protected function overviewTableRow($conditions, $id, $entity, $additional_cols = array()) {

    $created_at = format_date($entity->created_at, 'custom', 'j M Y, H:G');

    $mailing_list = moosend_get_active_lists_details(array($entity->list),FALSE);
    $mailing_list = l($mailing_list[$entity->list]['Name'],'https://vchouliaras.moosend.com/',
      array(
        'attributes' => array('target' => '_blank'),
        'fragment' => '/mailing-lists/'.$entity->list.'/view-list',
      )
    );

    $mode = NULL;
    switch ($entity->mode) {
      case MOOSEND_SUBSCRIPTIONS_BLOCK_DISPLAY:
        $mode = l(t('Block'), 'admin/structure/block');
        break;
      case MOOSEND_SUBSCRIPTIONS_PAGE_DISPLAY:
        $mode = t('Page');
        break;
      case MOOSEND_SUBSCRIPTIONS_BOTH_PAGE_BLOCK:
        $mode = l(t('Block'), 'admin/structure/block') . ' & ' . t('Page');
        break;
    }


    if (isset($entity->settings['custom_fields'])) {
        $customFields = array();
        foreach ($entity->settings['custom_fields'] as $key => $field) {
          $customFields[] = $field;
        }
        $customFields = !empty($customFields) ? implode(', ',$customFields) : t('None');
     }

    $additional_cols[] = $created_at;
    $additional_cols[] = $mailing_list;
    $additional_cols[] = $mode;
    $additional_cols[] = $customFields;

    return parent::overviewTableRow($conditions, $id, $entity, $additional_cols);
  }

}

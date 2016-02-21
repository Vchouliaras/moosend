<?php

/**
 * @file
 * Moosend Campaigns extending Entity API.
 */

/**
 * Extends Entity.
 */
class MoosendCampaigns extends Entity {

  /**
   * Override to implement a custom default URI and specify 'entity_class_uri' as 'uri callback' hook_entity_info().
   */
  protected function defaultUri() {
    return array(
      'path' => 'admin/config/services/moosend/campaigns/manage/' . $this->identifier(),
      'options' => array(
        'entity_type' => $this->entityType,
        'entity' => $this,
      ),
    );
  }

}

/**
 * Controller to extends EntityAPIController.
 */
class MoosendCampaignsController extends EntityAPIController {

  /**
   * Builds a structured array representing the entity's content.
   *
   * @see entity_build_content()
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $build = parent::buildContent($entity, $view_mode, $langcode, $content);

    return $build;
  }


  /**
   * Implements EntityAPIControllerInterface.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {

    if (isset($entity->is_new) && empty($entity->created_at)) {
      $entity->created_at = REQUEST_TIME;
    }
    $entity->updated_at = REQUEST_TIME;

    return parent::save($entity, $transaction);
  }


  /**
   * Implements EntityAPIControllerInterface.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    foreach ($ids as $id) {
      if (!moosend_delete_campaign($id)) {
        return FALSE;
      }
    }

    // Clear moosend_cache.
    moosend_get_campaings(1, 100, 'CreatedOn', 'ASC', FALSE);

    return parent::delete($ids, $transaction);
  }

}

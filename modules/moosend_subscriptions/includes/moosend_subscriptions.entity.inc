<?php

/**
 * @file
 * Moosend Subscription Module for extending Entity API.
 */

/**
 * Extend core Drupal Entity API.
 */
class MoosendSubscriptions extends Entity {

  /**
   * Override this in order to implement a custom default URI and specify.
   */
  protected function defaultUri() {
    return array(
      'path' => 'admin/config/services/moosend/subscriptions/manage/' . $this->identifier(),
      'options' => array(
        'entity_type' => $this->entityType,
        'entity' => $this,
      ),
    );
  }

}

/**
 * Extend contrib Drupal Entity API.
 */
class MoosendSubscriptionsController extends EntityAPIController {

  /**
   * Builds a structured array representing the entity's content.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $build = parent::buildContent($entity, $view_mode, $langcode, $content);

    return $build;
  }

  /**
   * Permanently saves the entity.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (isset($entity->is_new)) {
      $entity->created_at = REQUEST_TIME;
    }
    $entity->updated_at = REQUEST_TIME;

    return parent::save($entity, $transaction);
  }

}

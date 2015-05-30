<?php


class MoosendSubscriptions extends Entity {

  /**
   * Override this in order to implement a custom default URI and specify
   * 'entity_class_uri' as 'uri callback' hook_entity_info().
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


class MoosendSubscriptionsController extends EntityAPIController{

    public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $build = parent::buildContent($entity, $view_mode, $langcode, $content);

    // if ( $view_mode === 'member' ) {

    //   $build['embedcode'] = array(
    //     '#type' => 'markup',
    //     '#markup' => '<iframe width="560" height="315" src="http://www.youtube.com/embed/'. $entity->embedcode . '" frameborder="0" allowfullscreen></iframe>',
    //   );
    // }
    // else{

    //     $build['embedcode'] = array(
    //       '#type' => 'markup',
    //       '#markup' => '<p><a href="http://www.youtube.com/watch?v="'.$entity->embedcode.'">Link to Youtube Video</a></p>'
    //     );
    // }

    // $build['duration'] = array(
    //   '#type' => 'markup',
    //   '#markup' => format_interval($entity->duration),
    // );

    return $build;
  }


  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {

      if ( isset($entity->is_new) ) {
        $entity->created_at = REQUEST_TIME;
      }
      $entity->updated_at = REQUEST_TIME;
      return parent::save($entity,$transaction);
  }


}

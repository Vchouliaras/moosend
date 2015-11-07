<?php

/**
 * @file
 *  Drush integration for Moosend.
 *
 */

define(MOOSEND_WRAPPER_LINK, 'https://github.com/moosend/phpwrapper/archive/master.zip');

/**
 *  Implements hook_drush_command().
 *
 */
function moosend_drush_command() {
  $items = array();

  $items['moosend'] = array(
    'callback'  => '_moosend_drush_download_wrapper',
    'description' => dt('Downloads the Moosend PHP Wrapper from @github. Accepts an optional destination parameter.',
      array('@github' => MOOSEND_WRAPPER_LINK)),
    'aliases' => array('moo'),
    'arguments' => array(
      'path'  => dt('A destination for the Moosend PHP Wrapper library. If omitted Drush will use a default location.'),
    ),
  );

  return $items;
}


/**
 *  Drush Command Callback
 *
 */
function _moosend_drush_download_wrapper() {
  $args = func_get_args();

  if ($args[0]) {
    $dir = $args[0];
  }
  else {
    if (module_exists('libraries')) {
      $libraries_dir = libraries_get_path('moosend');
    }

    $drush_context = drush_get_context('DRUSH_DRUPAL_ROOT');
    if (!empty($libraries_dir)) {
      $dir = $drush_context . '/' . $libraries_dir;
    }
    else {
      $dir = $drush_context . '/sites/all/libraries/moosend';
    }
  }

  //Normalize path
  $dir = drush_normalize_path($dir);

  // Build full path including filename
  $path = _moosend_build_path($dir);

  // If the directory exists and contains a recognized file, we're finished.
  if (file_exists($path)) {
      drush_log('Moosend PHP Wrapper already installed. No download required.', 'ok');
      return;
  }
  else {
    // The file was not present. Create the directory to hold it.
    drush_mkdir($dir);

    //Move to $dir
    if (drush_op('chdir', $dir)) {
      if (drush_shell_exec('wget -O moosend.zip ' . MOOSEND_WRAPPER_LINK)) {
        drush_shell_exec('unzip moosend.zip -d .');
        drush_shell_exec('mv phpwrapper-master/* .');
        drush_delete_dir('phpwrapper-master');
        drush_delete_dir('moosend.zip');
        drush_log(dt('Drush download Moosend PHP Wrapper to @path', array('@path' => $dir)), 'success');
      }
      else {
        drush_log(dt('Drush was unable to download Moosend PHP Wrapper  to @path', array('@path' => $dir)), 'error');
      }
    }
  }
}


/**
 * Helper to get full library path
 *
 * @param
 *   string $dir
 * @return
 *   string
 */
function _moosend_build_path($dir) {
  return drush_trim_path($dir) . drush_normalize_path('/src/moosend/moosendAPI.php');
}

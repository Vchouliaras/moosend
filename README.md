# CONTENTS OF THIS FILE

 * Introduction
 * Requirements
 * Recommended modules
 * Installation
 * Configuration
 * Maintainers

## INTRODUCTION

The purpose of this module is to provide integration with Moosend, an email marketing and delivery service. Moosend module gives site builders and developers an easy way to interact with Moosend's capabilities. Those capabilities include user subscriptions, mailing and segment manipulation.

 * For a full description of the module, visit the project page:
   https://drupal.org/project/moosend

 * To submit bug reports and feature suggestions, or to track changes:
   https://drupal.org/project/issues/moosend


## REQUIREMENTS

This module requires the following modules:

 * Libraries (https://www.drupal.org/project/libraries)
 * Entity (https://www.drupal.org/project/entity)


## RECOMMENDED MODULES

 * Date (https://www.drupal.org/project/date):
    Provides date popups for Segment Criteria
 * Rules (https://www.drupal.org/project/rules)
 * View Bulk Operations (https://www.drupal.org/project/vbo)


## INSTALLATION

 * Install as you would normally install a contributed Drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.

 * Install Moosend PHP Wrapper. There is an official github.com repo located at
   [https://github.com/moosend/phpwrapper]([https://github.com/moosend/phpwrapper]).
   Download those files an put  them in `sites/all/libraries`. You can also you a drush command, it will download the PHP Wrapper in `sites/all/libraries`.
   ```sh
   $ drush moosend-wrapper
   ```

## CONFIGURATION

 * Got to `admin/config/services/moosend`. You will need to put in your Moosend API key for your Moosend account.
   If you do not have a Moosend API key, go to [http://www.moosend.com]([http://www.moosend.com) and sign up for a new account. You will also need to fill in your moosend prefix domain. i.e mydomain.moosend.com.

## MAINTAINERS

Current maintainers:
 * Vasileios Chouliaras (vchouliaras) - https://www.drupal.org/user/3069771

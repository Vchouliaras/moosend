<?php

/**
 * @file
 * Moosend Campaing API Wrapper.
 *
 */

/**
 * [moosend_get_campaings description]
 * @param  integer $page       [description]
 * @param  integer $page_size  [description]
 * @param  string  $sort_by     [description]
 * @param  string  $sort_method [description]
 * @param  boolean $cache      [description]
 * @return [type]              [description]
 */
function moosend_get_campaings($page = 1, $page_size = 100, $sort_by = 'CreatedOn', $sort_method = 'ASC', $cache = TRUE) {
  $campaings_details = $campaings_ids = array();

  if ($cache) {
   if (!empty(cache_get('campaigns', 'cache_moosend'))) {
    $campaings_ids = cache_get('campaigns', 'cache_moosend') -> data;
    foreach ($campaings_ids as $id) {
      $campaings_details[$id] = cache_get('campaign-' . $id, 'cache_moosend') -> data;
    }

    return $campaings_details;
    }
    else {
      moosend_get_campaings($page, $page_size, $sort_by, $sort_method, FALSE);
    }
  }
  try {
      $moosend_api_key = moosend_initialize_api();
      $campaigns = $moosend_api_key->campaigns->getCampaigns($page, $page_size, $sort_by, $sort_method);
      if (!empty($campaigns)) {
        cache_clear_all('campaign-', 'cache_moosend', TRUE);
        foreach ($campaigns as $campaign) {
          $campaigns_details[$campaign_id]['ID'] = $campaign_id = $campaign->getID();
          $campaigns_details[$campaign_id]['Name'] = $campaign->getName();
          $campaigns_details[$campaign_id]['Subject'] = $campaign->getSubject();
          $campaigns_details[$campaign_id]['SiteName'] = $campaign->getSiteName();
          $campaigns_details[$campaign_id]['ConfirmationTo'] = $campaign->getConfirmationTo();
          $campaigns_details[$campaign_id]['CreatedOn'] =_moosend_parse_date($campaign->getCreatedOn());
          $campaigns_details[$campaign_id]['HoursToTest'] = $campaign->getABHoursToTest();
          $campaigns_details[$campaign_id]['ABCampaignType'] = $campaign->getABCampaignType();
          $campaigns_details[$campaign_id]['ABWinnerSelectionType'] = $campaign->getABWinnerSelectionType();
          $campaigns_details[$campaign_id]['Status'] = $campaign->getStatus();
          $campaigns_details[$campaign_id]['DeliveredOn'] = _moosend_parse_date($campaign->getDeliveredOn());
          $campaigns_details[$campaign_id]['ScheduledFor'] = _moosend_parse_date($campaign->getScheduledFor());
          $campaigns_details[$campaign_id]['ScheduledForTimezone'] = $campaign->getScheduledForTimezone();
          $campaigns_details[$campaign_id]['MailingListID'] = $campaign->getMailingListID();
          $campaigns_details[$campaign_id]['MailingListName'] = $campaign->getMailingListName();
          $campaigns_details[$campaign_id]['SegmentID'] = $campaign->getSegmentID();
          $campaigns_details[$campaign_id]['SegmentName'] = $campaign->getSegmentName();
          $campaigns_details[$campaign_id]['MailingListStatus'] = $campaign->getMailingListStatus();
          $campaigns_details[$campaign_id]['TotalSent'] = $campaign->getTotalSent();
          $campaigns_details[$campaign_id]['TotalOpens'] = $campaign->getTotalOpens();
          $campaigns_details[$campaign_id]['UniqueOpens'] = $campaign->getUniqueOpens();
          $campaigns_details[$campaign_id]['TotalBounces'] = $campaign->getTotalBounces();
          $campaigns_details[$campaign_id]['TotalForwards'] = $campaign->getTotalForwards();
          $campaigns_details[$campaign_id]['UniqueForwards'] = $campaign->getUniqueForwards();
          $campaigns_details[$campaign_id]['TotalLinkClicks'] = $campaign->getTotalLinkClicks();
          $campaigns_details[$campaign_id]['UniqueLinkClicks'] = $campaign->getUniqueLinkClicks();
          $campaigns_details[$campaign_id]['RecipientsCount'] = $campaign->getRecipientsCount();
          $campaigns_details[$campaign_id]['IsTransactional'] = $campaign->getIsTransactional();
          $campaigns_details[$campaign_id]['TotalComplaints'] = $campaign->getTotalComplaints();
          $campaigns_details[$campaign_id]['TotalUnsubscribes'] = $campaign->getTotalUnsubscribes();

          //Populate $campaigns_details with more details about the campaigns
          $campaing_datails = $moosend_api_key->campaigns->getCampaign($campaign->getID());
          $campaigns_details[$campaign_id]['WebLocation'] = $campaing_datails->getWebLocation();
          $campaigns_details[$campaign_id]['HTMLContent'] = $campaing_datails->getHTMLContent();
          $campaigns_details[$campaign_id]['PlainContent'] = $campaing_datails->getPlainContent();
          $campaigns_details[$campaign_id]['WebLocation'] = $campaing_datails->getWebLocation();
          $campaigns_details[$campaign_id]['Sender'] = $campaing_datails->getSender()->getEmail();
          $campaigns_details[$campaign_id]['ReplyToEmail'] = $campaing_datails->getReplyToEmail()->getEmail();

          cache_set('campaign-' . $campaign_id, $campaigns_details[$campaign_id], 'cache_moosend');
        }
        cache_clear_all('campaigns', 'cache_moosend');
        cache_set('campaigns', array_keys($campaigns_details), 'cache_moosend');
      }

  return $campaigns_details;
  } catch (Exception $e) {
    watchdog('moosend', 'An error occurred retrieving details for Campaigns."%message"', array(
    '%message' => $e->getMessage()),
    WATCHDOG_ERROR);
  }
}





/**
 * [moosend_get_active_senders description]
 * @return [type] [description]
 */
function moosend_get_active_senders() {
  $moosend_senders = array();
  try {
      $moosend_api_key = moosend_initialize_api();
      $senders = $moosend_api_key->campaigns->getAllSenders();
      foreach ($senders as $key => $sender) {
        $moosend_senders[$sender_id]['ID'] = $sender_id = $sender->getID();
        $moosend_senders[$sender_id]['Name'] = $sender->getName();
        $moosend_senders[$sender_id]['Email'] = $sender->getEmail();
        $moosend_senders[$sender_id]['Email'] = $sender->getEmail();
        $moosend_senders[$sender_id]['CreatedOn'] = _moosend_parse_date($sender->getCreatedOn());
        $moosend_senders[$sender_id]['IsEnabled'] = $sender->getIsEnabled();
        $moosend_senders[$sender_id]['SpfVerified'] = $sender->getSpfVerified();
        $moosend_senders[$sender_id]['DkimVerified'] = $sender->getDkimVerified();
        $moosend_senders[$sender_id]['DkimPublic'] = $sender->getDkimPublic();
      }
    } catch (Exception $e) {
      watchdog('moosend', 'An error occurred retrieving Moosend acount senders."%message"', array(
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
    }
  return  $moosend_senders;
}


/**
 * [moosend_delete_campaign description]
 * @param  [type] $campaign_id [description]
 * @return [type]              [description]
 */
function moosend_delete_campaign($campaign_id) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
    $campaign_deleted = $moosend_api_key->campaigns->delete($campaign_id);
    $campaigns = moosend_get_campaings();
      watchdog('moosend', 'Campaing "@Name" was deleted succefully.',
      array('@Name' => $campaigns[$campaign_id]['Name']),
      WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred deleting Campaing "@Name" . "%message"',
      array('@Name' => $campaigns[$campaign_id]['Name'],
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * [moosend_create_campaign_draft description]
 * @param  [type] $params [description]
 * @return [type]         [description]
 */
function moosend_create_campaign_draft($params) {
  $campaign_id = '';
  if (!empty($params) && is_array($params)) {
    try{
      $moosend_api_key = moosend_initialize_api();
      $draft_params = new moosend\Models\CampaignParams();
      foreach ($params as $key => $value) {
        $draft_params->{$key} = $value;
      }

      $campaign_id = $moosend_api_key->campaigns->createDraft($draft_params);
    }
    catch (Exception $e) {
    watchdog('moosend', 'An error occurred creating draft Campaing "@Name"',
      array('@Name' => $draft_params->Name),
      WATCHDOG_ERROR);
    }
    return $campaign_id;
  }
}


/**
 * [moosend_update_campaign_draft description]
 * @param  [type] $campaign_id [description]
 * @param  [type] $params      [description]
 * @return [type]              [description]
 */
function moosend_update_campaign_draft($campaign_id, $params){
$status = TRUE;
$moosend_api_key = moosend_initialize_api();
$campaign = $moosend_api_key->campaigns->getCampaign($campaign_id);
 try{
      if(is_array($params)) {
        foreach ($params as $key => $value) {
          //Normalize specific keys and their values
          if (strstr($key, 'Sender')) {
            $value = $moosend_api_key->campaigns->getSenderDetails($params['SenderEmail']);
            $key = 'Sender';
          }
          elseif (strstr($key, 'ReplyToEmail')) {
            $value = $moosend_api_key->campaigns->getSenderDetails($params['ReplyToEmail']);
          }
          elseif (strstr($key, 'Segment')) {
            $value = $moosend_api_key->segments->getDetails($params['MailingListID'], $params['SegmentID']);
            $key = 'Segment';
          }
          elseif (strstr($key, 'MailingList')) {
            $value = $moosend_api_key->mailingLists->getDetails($params['MailingListID']);
            $key = 'MailingList';
          }
          elseif (strstr($key, 'Confirmation')) {
            $key = 'ConfirmationTo';
          }

          //Call the Campaign method
          $method = 'set' . ucfirst($key);
          $campaign->$method($value);
        }
      }

    $moosend_api_key->campaigns->updateDraft($campaign);
    }
    catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred updating draft Campaing @Name. "@message"',
      array('@Name' => $campaign->getName(),'@message' => $e->getMessage()),
      WATCHDOG_ERROR);
    }
  return $status;
}

// setName();
// setStatus();
// setSubject();
// setReplyToEmail(Sender $sender)
// setMailingList(MailingList $mailingList)
// setWebLocation($webLocation)
// setSender(Sender $sender)
// setConfirmationTo($confirmationTo)
// setSegment(Segment $segment)




/**
 * [moosend_send_campaign description]
 * @param  [type] $campaign_id [description]
 * @return [type]              [description]
 */
function moosend_send_campaign($campaign_id) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
    $moosend_api_key->campaigns->send($campaign_id);
    $campaigns = moosend_get_campaings();
    watchdog('moosend', 'A Campaign named "@Name" was sent succefully.',
    array('@Name' => $campaigns[$campaign_id]['Name']),
    WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred sending Campaign "@Name" . "%message"',
      array('@Name' => $campaigns[$campaign_id]['Name'],
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * [moosend_send_test_campaign description]
 * @param  [type] $campaign_id [description]
 * @param  [type] $email       [description]
 * @return [type]              [description]
 */
function moosend_send_test_campaign($campaign_id, $emails) {
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
      $moosend_api_key->campaigns->sendCampaignTest($campaign_id, $emails);
      $campaigns = moosend_get_campaings();
      watchdog('moosend', 'A test Campaign named "@Name" was sent succefully.',
      array('@Name' => $campaigns[$campaign_id]['Name']),
      WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error occurred sending test Campaign "@Name" . "%message"',
      array('@Name' => $campaigns[$campaign_id]['Name'],
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $status;
}



function moosend_campaign_clone($campaign_id){
  $status = TRUE;
  try {
    $moosend_api_key = moosend_initialize_api();
    $clone = $moosend_api_key->campaigns->cloneCampaign($campaign_id);
    watchdog('moosend', 'The campaign "@Name" was cloned succefully.',
    array('@Name' => $clone->getName()),
    WATCHDOG_NOTICE);
  }
  catch (Exception $e) {
    $status = FALSE;
    watchdog('moosend', 'An error cloning campaign "@Name" . "%message"',
      array('@Name' => $clone->getName(),
      '%message' => $e->getMessage()),
      WATCHDOG_ERROR);
  }

  return $status;
}


/**
 * [moosend_get_campaign_status_name description]
 * @param  [type] $code [description]
 * @return [type]       [description]
 */
function moosend_get_campaign_status_name($code) {
  $status = NULL;
  switch ($code) {
    case CAMPAIGN_DRAFT:
      $status = t('Draft');
      break;
    case CAMPAIGN_READY_TO_SEND:
    case CAMPAIGN_SENDING:
    case CAMPAIGN_SEND:
      $status = t('Sent');
      break;
    case CAMPAIGN_NOT_ENOUGH_CREDITS:
      $status = t('No Credits');
      break;
    case CAMPAIGN_SELECTING_WINNER:
      $status = t('Selecting Winner');
    break;
    case CAMPAIGN_ARCHIVED:
      $status = t('Archived');
    break;
  }

  return $status;
}

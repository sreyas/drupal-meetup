<?php
/**
 * @file
 * Provides API integration with the Meetup microblogging service.
 */

// require_once libraries_get_path('meetup_login') . '/Meetup.php';
/**
 * Delete form for Meetup account.
 */
function meetup_login_delete_form($form, &$form_state, $id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  return confirm_form($form,
    t('Are you sure you want to delete'),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/services/meetup/delete' . $id,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}
/**
 * Unlink form for Meetup account.
 */
function meetup_login_unlink_form($form, &$form_state, $id) {
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  return confirm_form($form,
    t('Are you sure you want to unlink'),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/services/meetup/unlink' . $id,
    t('This action cannot be undone.'),
    t('Unlink'),
    t('Cancel')
  );
}
/**
 * Form submit for the Meetup Unlink.
 */
function meetup_login_unlink_form_submit($form, &$form_state) {
  meetup_login_unlink_confirm($form_state['values']['id']);
  $form_state['redirect'] = 'admin/config/services/meetup/';
}
/**
 * Form submit for the Meetup delete.
 */
function meetup_login_delete_form_submit($form, &$form_state) {
  meetup_login_delete_confirm($form_state['values']['id']);
  $form_state['redirect'] = 'admin/config/services/meetup/';
}


/**
 * Confirmation form  of meetup delete form.
 */
function meetup_login_delete_confirm($id) {
  $uid = db_select("meetup_account", "ma")
          ->fields("ma", array("uid"))
          ->condition('meetup_uid', $id)
          ->execute()
          ->fetchField();
  user_delete($uid);
  db_delete('meetup_account')
    ->condition('meetup_uid', $id)
    ->execute();
}
/**
 * Confirmation form  of meetup Unlink form.
 */
function meetup_login_unlink_confirm($id) {
  db_delete('meetup_account')
    ->condition('meetup_uid', $id)
    ->execute();
}
/**
 * Page callbacks for Meetup Signin.
 */
function meetup_login_redirect() {
  $libpath = libraries_get_path('meetup_login') . '/Meetup.php';
  $localpath = variable_get('meetup_login_local_path');
  $path = $localpath . '/Meetup.php';
  if (file_exists($path)) {
    require "$path";
  }
  else {
    require "$libpath";
  }
  $client_id = variable_get('meetup_login_consumer_key', '');
  $redirect_uri = variable_get('meetup_login_redirect_url', '');
  // Begin a new session if there isn't an existing one.
  unset($_SESSION['meetup_access_token']);
  if (!session_id()) {
    session_start();
  }
  // If the access token is already stored in the session, we're good.
  if (isset($_SESSION['meetup_access_token'])) {
  }
  else {
    // The redirect URI is where you want your user to be redirected after.
    // Meetup confirms that your consumer is valid.
    MeetupOAuth2Helper::request_auth($client_id, $redirect_uri);
  }
}
/**
 * Page callbacks for Meetup accounts and settings.
 */
function meetup_login_account_settings() {
  $img = drupal_get_path('module', 'meetup_login') . '/images/delete.png';
  $header = array();
  $header[] = 'Name';
  $header[] = 'Id';
  $header[] = 'Actions';
  $rows = array();
  $results = db_select('meetup_account', 'ma')->fields('ma')->execute();
  foreach ($results as $res) {
    $row = array();
    $unlink = l(t('Unlink'), 'admin/config/services/meetup/unlink/' . $res->meetup_uid);
    $dellink = l(theme('image', array('path' => $img, 'alt' => t('Delete  Meetup Accounts'))), 'admin/config/services/meetup/delete/' . $res->meetup_uid, array('html' => TRUE));
    $row[] = $res->name;
    $row[] = $res->id;
    $row[] = $dellink . '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;' . $unlink;
    $rows[] = $row;
  }
  if (!empty($rows)) {
    return theme('table', array('header' => $header, 'rows' => $rows));
  }
  else {
    return t('No Meetup accounts have been added yet');
  }
}
/**
 * Meetup settings form.
 */
function meetup_login_admin_form($form, &$form_state) {
  $form['oauth'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth Settings'),
    '#description' => t('To enable OAuth based access for meetup, you must <a href="@url">register your application</a> with meetup and add the provided keys here.', array('@url' => 'http://www.meetup.com/meetup_api/oauth_consumers/')),
  );
  $form['oauth']['callback_url'] = array(
    '#type' => 'item',
    '#title' => t('Callback URL'),
    '#markup' => url('meetup/oauth', array('absolute' => TRUE)),
  );
  $form['oauth']['meetup_login_local_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Local path to meetup library'),
    '#default_value' => variable_get('meetup_login_local_path', NULL),
  );
  $form['oauth']['meetup_login_consumer_key'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('OAuth Consumer key'),
    '#default_value' => variable_get('meetup_login_consumer_key', NULL),
  );
  $form['oauth']['meetup_login_consumer_secret'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('OAuth Consumer secret'),
    '#default_value' => variable_get('meetup_login_consumer_secret', NULL),
  );
  $form['oauth']['meetup_login_group_url'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Enter group url name'),
    '#default_value' => variable_get('meetup_login_group_url', NULL),
  );
  $form['oauth']['meetup_login_redirect_url'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Redirect url'),
    '#default_value' => variable_get('meetup_login_redirect_url', NULL),
  );
  // Meetup external APIs settings.
  $form['meetup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Meetup Settings'),
    '#description' => t('The following settings connect Meetup module with external APIs
      Change them if, for example, you want to use Identi.ca.'),
  );
  $form['meetup']['meetup_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup host'),
    '#default_value' => variable_get('meetup_login_host', MEETUP_LOGIN_MEETUP_HOST),
  );
  $form['meetup']['meetup_api'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup API'),
    '#default_value' => variable_get('meetup_login_api', MEETUP_LOGIN_MEETUP_API),
  );
  $form['meetup']['meetup_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetup search'),
    '#default_value' => variable_get('meetup_login_search', MEETUP_LOGIN_MEETUP_SEARCH),
  );
  $form['meetup']['meetup_tinyurl'] = array(
    '#type' => 'textfield',
    '#title' => t('TinyURL'),
    '#default_value' => variable_get('meetup_login_tinyurl', MEETUP_LOGIN_MEETUP_TINYURL),
  );
  // Meetup Field mapping settings.
  $form['meetup_map'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field Mapping'),
    '#description' => t('Each of your account fields are listed below. Map each fields with your meetup data fields.'),
  );
  $instances = field_info_instances('user', 'user');
  $options = meetup_login_user_properties();
  foreach ($instances as $field_name => $instance) {
    $default = meetup_login_mapping_getting_default_value($field_name, $options);
    $form['meetup_map'][$instance['bundle']][$field_name] = array(
      '#type' => 'select',
      '#title' => check_plain($instance['label']),
      '#options' => $options,
      '#default_value' => variable_get($field_name, $default),
    );
  }
  return system_settings_form($form);
}
/**
 * Form validation.
 */
function meetup_login_admin_form_validate($form, $form_state) {
  $meetup_library = preg_replace("/(^[\r\n]*|[\r\n]+)[\s\t]*[\r\n]+/", "\n", trim($form_state['values']['meetup_login_local_path']));
  $link = 'Meetup.php';
  $file = $meetup_library . '/' . $link;
  if (!file_exists($file)) {
    form_set_error('meetup_login_local_path', t('You must enter a correct path.'));
  }
}
/**
 * Return a default mapping field.
 */
function meetup_login_mapping_getting_default_value($id, $array) {
  $pos = meetup_login_strpos_array($id, $array);
  return $pos;
}
/**
 * Callback function.
 */
function meetup_login_strpos_array($haystack, $needle) {
  if (!is_array($needle)) {
    $needle = array($needle);
    foreach ($needle as $key => $what) {
      if ((strpos($haystack, $key)) !== FALSE) {
        return $key;
      }
    }
  }
}
/**
 * Wrapper to call drupal_form_submit().
 */
function meetup_login_oauth_callback() {
  // Begin a new session if there isn't an existing one.
  if (!session_id()) {
    session_start();
  }
  $libpath = libraries_get_path('meetup_login') . '/Meetup.php';
  $localpath = variable_get('meetup_login_local_path');
  $path = $localpath . '/Meetup.php';
  if (file_exists($path)) {
    require "$path";
  }
  else {
    require "$libpath";
  }
  $client_id = variable_get('meetup_login_consumer_key', '');
  $client_secret = variable_get('meetup_login_consumer_secret', '');
  $redirect_uri = variable_get('meetup_login_redirect_url', '');
  // Meetup will always send back a code in the $_GET parameters upon success.
  // This is a one-time code used to get an access token.
  if (isset($_GET['code'])) {
    $response_obj = MeetupOAuth2Helper::request_access_token(array(
        "client_id" => $client_id,
        "client_secret" => $client_secret,
        "redirect_uri" => $redirect_uri ,
        "code" => $_GET['code'],
        "user_agent" => "",
    ));
    $_SESSION['meetup_access_token'] = $response_obj;
    variable_set('meetup_login_secret_information', $_SESSION['meetup_access_token']);
    drupal_goto('meetup_token');
  }
}
/**
 * Wrapper to call drupal_form_submit().
 */
function meetup_login_token_callback() {
  $secret_token = variable_get('meetup_login_secret_information', '');
  if (isset($_GET['denied']) || empty($secret_token)) {
    drupal_set_message(t('The connection to Meetup failed. Please try again.'), 'error');
    global $user;
    if ($user->uid) {
      // User is logged in, was attempting to OAuth a Meetup account.
      drupal_goto('admin/config/services/meetup');
    }
    else {
      // Anonymous user, redirect to front page.
      drupal_goto('<front>');
    }
  }
  $form_state['values']['oauth_token'] = $secret_token;
  drupal_form_submit('meetup_login_oauth_callback_form', $form_state);
}
/**
 * This form was built in response to the oauth return request from Meetup.
 */
function meetup_login_oauth_callback_form($form, &$form_state) {
  $secret_token = variable_get('meetup_login_secret_information', '');
  $form['#post']['oauth_token'] = $secret_token;
  $form['oauth_token'] = array(
    '#type' => 'hidden',
    '#default_value' => $secret_token,
  );
  return $form;
}

/**
 * Validate results from Meetup OAuth return request.
 */
function meetup_login_oauth_callback_form_validate($form, &$form_state) {
  $libpath = libraries_get_path('meetup_login') . '/Meetup.php';
  $localpath = variable_get('meetup_login_local_path');
  $path = $localpath . '/Meetup.php';
  if (file_exists($path)) {
    require "$path";
  }
  else {
    require "$libpath";
  }
  $secret_token = variable_get('meetup_login_secret_information', '');
  if (isset($secret_token)) {
    $form_state['meetup_oauth'] = $secret_token;
    unset($_SESSION['meetup_access_token']);
  }
  else {
    form_set_error('oauth_token', t('Invalid Meetup OAuth request'));
  }
  $accesstoken = $form_state['meetup_oauth'];
  $connection = new MeetupOAuth2Connection($accesstoken);
  if ($accesstoken) {
    $url_self = 'https://api.meetup.com/2/member/self?access_token=' . $accesstoken;
    $page = file_get_contents($url_self);
    $results = json_decode($page, TRUE);
    $memberid = $results['id'];
    $group_url_name = variable_get('meetup_login_group_url');
    $members = new MeetupMembers($connection);
    $member_profile = $members->getMembers(array('member_id' => $memberid));
    $groups = $members->getProfiles(array('group_urlname' => $group_url_name, 'member_id' => $memberid));
    $group_photo_link = isset($groups[0]['photo_url']) ? $groups[0]['photo_url'] : '';
    $profile_photo_link = isset($member_profile[0]['photo']) ? $member_profile[0]['photo']['photo_link'] : '';
    $name = array("name" => $results['name']);
    $upper_country = $results['country'];
    $upper_country = strtoupper($upper_country);
    $country = array('country' => $upper_country);
    $city = array('city' => $results['city']);
    $photo_url = isset($group_photo_link) ? $group_photo_link : $profile_photo_link;
    $photo_url = array('photo_url' => $photo_url);
    $twitter_identifier = array('twitter' => $results['other_services']['twitter']['identifier']);
    $flicker_identifier = array('flickr' => $results['other_services']['flickr']['identifier']);
    $facebook_identifier = array('facebook' => $results['other_services']['facebook']['identifier']);
    $memberid = array('member_id' => $memberid);
    $accesstoken = array('oauth_token' => $accesstoken);
    $response = array_merge($name, $country, $city, $photo_url, $twitter_identifier, $flicker_identifier, $facebook_identifier, $memberid, $accesstoken);
    $form_state['response'] = $response;
  }
  else {
    form_set_error('oauth_token', t('Invalid Meetup OAuth request'));
  }
}
/**
 * Form submit for the  Meetup_login_oauth_callback.
 */
function meetup_login_oauth_callback_form_submit($form, &$form_state) {
  global $user;
  $uid = $user->uid;
  $account = user_load($uid);
  $mail = $account->mail;
  $response = $form_state['response'];
  meetup_login_account_save($response, TRUE);
  $form_state['programmed'] = FALSE;
  if ($mail == NULL) {
    $destination = '/user/' . $uid . '/edit';
    $query = array('meetup' => 'useredit');
  }
  else {
    $destination = '/user/' . $uid . '/view';
    $query = array('meetup' => 'userview');
  }
  $form_state['redirect'] = array(
    $destination,
    array(
      'query' => $query,
    ),
  );
}
/**
 * Return a list of Meetup user properties.
 */
function meetup_login_user_properties() {
  $common = array(
    '_none' => t('Select'),
    'member_id' => t('Member ID'),
    'zip' => t('Zip'),
    'country' => t('Country'),
    'state' => t('State'),
    'city' => t('City'),
    'name' => t('Name'),
    'facebook' => t('Facebook'),
    'twitter' => t('Twitter'),
    'flickr' => t('Flicker'),
  );
  return $common;
}
/**
 * Saves a Meetup members object to meetup_account.
 */
function meetup_login_account_save($meetup_user, $save_auth = FALSE) {
  global $user;
  $uid = $user->uid;
  $values = (array) $meetup_user;
  $user_id = isset($values['uid']) ? $values['uid'] : $uid;
  if (isset($values['member_id'])) {
    $meetup_id = db_select('meetup_account', 'ma')
      ->fields('ma', array('meetup_uid'))
      ->condition('id', $values['member_id'], '=')
      ->execute()
     ->fetchAssoc();
    if (empty($meetup_id)) {
      db_insert('meetup_account')
        ->fields(array(
          'oauth_token' => $values['oauth_token'] ,
          'name' => $values['name'],
          'country' => $values['country'],
          'city' => $values['city'],
          'uid' => $user_id,
          'id' => $values['member_id'],
        ))
       ->execute();
    }
  }
}

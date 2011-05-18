<?php
// $Id$

/**
 * @file
 * Drush commands for Bot module. Inserts the IRC library into Drupal's hook
 * system.
 */

/**
 * Implementation of hook_drush_command().
 */
function bot_drush_command() {
  $items = array();

  $items['bot-start'] = array(
    'description' => 'Starts the IRC bot for this site.',
    'examples' => array(
      'drush bot-start &' => 'Connect the IRC bot as a background process.',
      'nohup drush bot-start &' => 'Connect the IRC bot as a background process using nohup.',
    ),
  );
  $items['bot-stop'] = array(
    'description' => 'Stops the IRC bot for this site.',
    'options' => array(
      '--timeout' => 'Number of seconds to attempt the disconnect. Defaults to 45.',
    ),
    'examples' => array(
      'drush bot-stop --timeout="60"' => 'Disconnect the IRC bot, allowing 60 seconds of processing time.',
    ),
  );
  $items['bot-status'] = array(
    'description' => 'Returns the IRC bot connection state for this site.',
  );
  $items['bot-reset-status'] = array(
    'description' => 'Resets the IRC bot connection state for this site.',
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 */
function bot_drush_help($section) {
  switch ($section) {
    case 'drush:bot-start':
      return dt('Starts the IRC bot for this site. It is recommended to start it as a background process.');
    case 'drush:bot-stop':
      return dt('Stops the IRC bot for this site.');
    case 'drush:bot-status':
      return dt('Returns the IRC bot connection state for this site.');
    case 'drush:bot-reset-status':
      return dt('Resets the IRC bot connection state for this site.');
  }
}

/**
 * Callback for drush bot-start.
 */
function drush_bot_start() {
  $status = _bot_get_status();
  if ($status == BOT_STATUS_DISCONNECTED) {
    _drush_bot_start();
  }
  else {
    _bot_drush_log_status($status, 'warning');
  }
}

/**
 * Does the heavy lifting of starting the IRC bot.
 */
function _drush_bot_start() {
  drush_log(dt('The IRC bot is connecting.'));
  variable_set('bot_status', BOT_STATUS_CONNECTING);
  // prevent MySQL timeouts on slow channels.
  db_query('SET SESSION wait_timeout = %d', 24*60*60);

  require_once('Net/SmartIRC.php');

  // initialize the bot with some sane defaults.
  global $irc; // allow it to be slurped by Drupal modules if need be.
  $bot = new drupal_wrapper(); // wrapper that integrates with Drupal hooks.
  $irc = new Net_SmartIRC(); // MmmmmmM. The IRC object itself. Magick happens here.

  // Net_SmartIRC::nreplycodes is set from $_GLOBALS, which is unreliable.
  $irc->nreplycodes = $SMARTIRC_nreplycodes;
  $irc->setDebug( variable_get('bot_debugging', 0) ? SMARTIRC_DEBUG_ALL : SMARTIRC_DEBUG_NONE );
  // the (boolean) here is required, as Net_SmartIRC doesn't respect a FAPI checkbox value of 1, only TRUE.
  $irc->setAutoReconnect((boolean) variable_get('bot_auto_reconnect', 1)); // reconnect to the server if disconnected.
  $irc->setAutoRetry((boolean) variable_get('bot_auto_retry', 1)); // retry if a server connection fails.
  $irc->setUseSockets((boolean) variable_get('bot_real_sockets', 1)); // socket_connect or fsockopen?
  $irc->setChannelSyncing(TRUE); // keep a list of joined users per channel.

  // send every message type the library supports to our wrapper class.
  // we can automate the creation of these actionhandlers, but not the
  // class methods below (only PHP 5 supports default methods easily).
  $irc_message_types = array(
    'UNKNOWN',    'CHANNEL', 'QUERY',    'CTCP',        'NOTICE',       'WHO',
    'JOIN',       'INVITE',  'ACTION',   'TOPICCHANGE', 'NICKCHANGE',   'KICK',
    'QUIT',       'LOGIN',   'INFO',     'LIST',        'NAME',         'MOTD',
    'MODECHANGE', 'PART',    'ERROR',    'BANLIST',     'TOPIC',        'NONRELEVANT',
    'WHOIS',      'WHOWAS',  'USERMODE', 'CHANNELMODE', 'CTCP_REQUEST', 'CTCP_REPLY',
  );

  foreach ($irc_message_types as $irc_message_type) {
    $irc->registerActionhandler(constant('SMARTIRC_TYPE_' . $irc_message_type), '.*', $bot, 'invoke_irc_msg_' . strtolower($irc_message_type));
  }

  // set up a timers similar to Drupal's hook_cron(), multiple types. I would have
  // liked to just pass a parameter to a single function, but SmartIRC can't do that.
  $irc->registerTimehandler(300000, $bot, 'invoke_irc_bot_cron');           // 5 minutes.
  $irc->registerTimehandler(60000, $bot,  'invoke_irc_bot_cron_faster');    // 1 minute.
  $irc->registerTimehandler(15000, $bot,  'invoke_irc_bot_cron_fastest');   // 15 seconds.

  // connect and begin listening.
  $irc->connect(variable_get('bot_server', 'irc.freenode.net'), variable_get('bot_server_port', 6667));
  $irc->login(variable_get('bot_nickname', 'bot_module'), variable_get('bot_nickname', 'bot_module') . ' :http://drupal.org/project/bot', 8, variable_get('bot_nickname', 'bot_module'), (variable_get('bot_password', '') != '') ? variable_get('bot_password', '') : NULL);

  // channel joining has moved to bot_irc_bot_cron_fastest().
  // read that function for the rationale, and what we gain from it.

  $irc->listen();
  $irc->disconnect(); // if we stop listening, disconnect properly.
  variable_del('bot_status');
  drush_log(dt('The IRC bot has disconnected.'));
}

/**
 * Callback for drush bot-stop.
 */
function drush_bot_stop() {
  $status = _bot_get_status();
  switch ($status) {
    case BOT_STATUS_DISCONNECTING:
      drush_log(dt('The bot is already disconnecting.'), 'warning');
      break;

    case BOT_STATUS_DISCONNECTED:
      drush_log(dt('The bot is already disconnected.'), 'warning');
      break;

    default:
      _drush_bot_stop();
  }
}

/**
 * Queue the IRC bot for stopping.
 */
function _drush_bot_stop() {
  drush_log(dt('Disconnecting the IRC bot.'), 'ok');

  $timeout = drush_get_option('timeout', 45);
  $status = BOT_STATUS_DISCONNECTING;

  variable_set('bot_status', $status);
  lock_acquire('bot_disconnect', $timeout);

  // Continue this loop until the lock expires or the bot disconnects.
  while (!($status == BOT_STATUS_DISCONNECTED || lock_may_be_available('bot_disconnect'))) {
    // Check the status every 5 seconds.
    sleep(5);
    $status = _bot_get_status();
    _bot_drush_log_status($status);
  }

  $type = 'error';
  if ($status == BOT_STATUS_DISCONNECTED) {
    lock_release('bot_disconnect');
    $type = 'success';
  }
  _bot_drush_log_status($status, $type);
}

/**
 * Callback for drush bot-status.
 */
function drush_bot_status() {
  // If we are in the backend context, print the status directly.
  if (drush_get_context('DRUSH_BACKEND')) {
    $status = variable_get('bot_status', BOT_STATUS_DISCONNECTED);
    drush_print($status);
    return;
  }
  else {
    $status = _bot_get_status();
  }

  _bot_drush_log_status($status, 'ok');
}

/**
 * Retrieve the current bot status, clearing the variable cache first.
 */
function _bot_get_status() {
  // We first clear the variables cache and then invoke the command to get a
  // fresh status. Otherwise the static variable may contain stale data.
  cache_clear_all('variables', 'cache');
  $ret = drush_backend_invoke('bot-status', array(), 'GET', FALSE);
  return $ret['output'];
}

/**
 * Helper function to log the current status to drush.
 *
 * @param $status
 *   The status to log.
 * @param $type
 *   The type of message to be logged. Common types are 'warning', 'error',
 *   'success' and 'notice'. Defaults to 'notice'.
 */
function _bot_drush_log_status($status, $type = 'notice') {
  switch ($status) {
    case BOT_STATUS_DISCONNECTED:
      drush_log(dt('The bot is currently disconnected.'), $type);
      break;

    case BOT_STATUS_CONNECTING:
      drush_log(dt('The bot is currently connecting.'), $type);
      break;

    case BOT_STATUS_CONNECTED:
      drush_log(dt('The bot is currently connected.'), $type);
      break;

    case BOT_STATUS_DISCONNECTING:
      drush_log(dt('The bot is currently disconnecting.'), $type);
      break;
  }
}

/**
 * Callback for drush bot-reset-status
 */
function drush_bot_reset_status() {
  // We check locks first to be double sure that this is safe.
  if (lock_may_be_available('bot_disconnect') && drush_confirm(dt('Are you sure you want to reset the IRC bot status?'))) {
    variable_del('bot_status');
    drush_log(dt('The bot connection status has been reset.'), 'ok');
  }
  else {
    drush_log(t('It appears the bot is currently disconnecting. Please wait.'), 'warning');
  }
}

// pass off IRC messages to our modules via Drupal's hook system.
class drupal_wrapper {
  function invoke_irc_bot_cron(&$irc)                 { module_invoke_all('irc_bot_cron'); }
  function invoke_irc_bot_cron_faster(&$irc)          { module_invoke_all('irc_bot_cron_faster'); }
  function invoke_irc_bot_cron_fastest(&$irc)         { module_invoke_all('irc_bot_cron_fastest'); }
  function invoke_irc_msg_unknown(&$irc, &$data)      { module_invoke_all('irc_msg_unknown', $data); }
  function invoke_irc_msg_channel(&$irc, &$data)      { module_invoke_all('irc_msg_channel', $data); }
  function invoke_irc_msg_query(&$irc, &$data)        { module_invoke_all('irc_msg_query', $data); }
  function invoke_irc_msg_ctcp(&$irc, &$data)         { module_invoke_all('irc_msg_ctcp', $data); }
  function invoke_irc_msg_notice(&$irc, &$data)       { module_invoke_all('irc_msg_notice', $data); }
  function invoke_irc_msg_who(&$irc, &$data)          { module_invoke_all('irc_msg_who', $data); }
  function invoke_irc_msg_join(&$irc, &$data)         { module_invoke_all('irc_msg_join', $data); }
  function invoke_irc_msg_invite(&$irc, &$data)       { module_invoke_all('irc_msg_invite', $data); }
  function invoke_irc_msg_action(&$irc, &$data)       { module_invoke_all('irc_msg_action', $data); }
  function invoke_irc_msg_topicchange(&$irc, &$data)  { module_invoke_all('irc_msg_topicchange', $data); }
  function invoke_irc_msg_nickchange(&$irc, &$data)   { module_invoke_all('irc_msg_nickchange', $data); }
  function invoke_irc_msg_kick(&$irc, &$data)         { module_invoke_all('irc_msg_kick', $data); }
  function invoke_irc_msg_quit(&$irc, &$data)         { module_invoke_all('irc_msg_quit', $data); }
  function invoke_irc_msg_login(&$irc, &$data)        { module_invoke_all('irc_msg_login', $data); }
  function invoke_irc_msg_info(&$irc, &$data)         { module_invoke_all('irc_msg_info', $data); }
  function invoke_irc_msg_list(&$irc, &$data)         { module_invoke_all('irc_msg_list', $data); }
  function invoke_irc_msg_name(&$irc, &$data)         { module_invoke_all('irc_msg_name', $data); }
  function invoke_irc_msg_motd(&$irc, &$data)         { module_invoke_all('irc_msg_motd', $data); }
  function invoke_irc_msg_modechange(&$irc, &$data)   { module_invoke_all('irc_msg_modechange', $data); }
  function invoke_irc_msg_part(&$irc, &$data)         { module_invoke_all('irc_msg_part', $data); }
  function invoke_irc_msg_error(&$irc, &$data)        { module_invoke_all('irc_msg_error', $data); }
  function invoke_irc_msg_banlist(&$irc, &$data)      { module_invoke_all('irc_msg_banlist', $data); }
  function invoke_irc_msg_topic(&$irc, &$data)        { module_invoke_all('irc_msg_topic', $data); }
  function invoke_irc_msg_nonrelevant(&$irc, &$data)  { module_invoke_all('irc_msg_nonrelevant', $data); }
  function invoke_irc_msg_whois(&$irc, &$data)        { module_invoke_all('irc_msg_whois', $data); }
  function invoke_irc_msg_whowas(&$irc, &$data)       { module_invoke_all('irc_msg_whowas', $data); }
  function invoke_irc_msg_usermode(&$irc, &$data)     { module_invoke_all('irc_msg_usermode', $data); }
  function invoke_irc_msg_channelmode(&$irc, &$data)  { module_invoke_all('irc_msg_channelmode', $data); }
  function invoke_irc_msg_ctcp_request(&$irc, &$data) { module_invoke_all('irc_msg_ctcp_request', $data); }
  function invoke_irc_msg_ctcp_reply(&$irc, &$data)   { module_invoke_all('irc_msg_ctcp_reply', $data); }
}

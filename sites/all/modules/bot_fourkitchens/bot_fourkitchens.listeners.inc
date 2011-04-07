<?php

/**
 * Summon
 *
 * @command ?summon <message>
 */
function bot_fourkitchens_summon($to, $data, $matches) {
  global $irc;
  bot_message($to, implode(', ', array_keys($irc->channel[$data->channel]->users)) . ' ' . $matches[1]);
}

/**
 * Search Google
 *
 * @command ?g <query>
 */
function bot_fourkitchens_google($to, $data, $matches) {
  if (preg_match('/^\?g\s(.*)$/', $data->message, $matches)) {
    $google_url = 'http://ajax.googleapis.com/ajax/services/search/web?v=1.0&q=' . urlencode($matches[1]);
    $response = drupal_http_request($google_url, array('User-Agent' => 'Drupal IRC Bot +(http://drupal.com/project/bot)'));
    if ($response->code == 200 && ($results = json_decode($response->data))) {
      bot_message($to, $data->nick . ': ' . $results->responseData->results[0]->url . ' - ' . strip_tags($results->responseData->results[0]->content));
    }
  }
}
  
/**
 * Search Urban Dictionary
 *
 * @command ?ud <query>
 */
function bot_fourkitchens_urban_dictionary($to, $data, $matches) {
  bot_fourkitchens_include_simple_html_dom();
  $html = file_get_html('http://www.urbandictionary.com/define.php?term=' . urlencode($matches[1]));
  $term = $html->find('#entries tr', 0);
  $def = $html->find('#entries tr .definition', 1);
  $term = preg_replace('/\s\s+/', ' ', trim($term->plaintext));
  $def = preg_replace('/\s\s+/', ' ', trim($def->plaintext));
  if ($term && $def) {
    bot_message($to, $data->nick . ": $term - $def");
  }
}
  
/**
 * Get the current weather
 *
 * @command ?w <location>
 */
function bot_fourkitchens_weather($to, $data, $matches) {
  $url = 'http://www.google.com/ig/api?weather=' . urlencode($matches[1]);
  $xml = file_get_contents($url);
  if ($xml) {
    $xml = new SimpleXMLElement($xml);
    if (count($xml->weather->children()) > 1) {
      $condition = (string) $xml->weather->current_conditions->condition['data'];
      $temp = (string) $xml->weather->current_conditions->temp_f['data'];
      $temp_c = (string) $xml->weather->current_conditions->temp_c['data'];
      $humidity = (string) $xml->weather->current_conditions->humidity['data'];
      $wind = (string) $xml->weather->current_conditions->wind_condition['data'];
      bot_message($to, sprintf("%s: %s, %s | %s F / %s C | %s | %s", 
        $data->nick, $matches[1], $condition, $temp, $temp_c, $humidity, $wind));
    }
  }
}

/**
 * What would Jen-nay do?
 */
function bot_fourkitchens_jenny($to, $data, $matches) {
  $things = bot_fourkitchens_listener_data();
  bot_message($to, $things[rand(0, count($things) - 1)]);
}

/**
 * Jenny we've got your number
 */
function bot_fourkitchens_jennys_number($to, $data, $matches) {
  bot_message($to, '8675309');
}

/**
 * Jira issues
 */
function bot_fourkitchens_jira($to, $data, $matches) {
  $projects = bot_fourkitchens_listener_data();
  if (in_array($matches[1], $projects)) {
    bot_message($to, $data->nick . ': JIRA issue: https://projects.fourkitchens.com/browse/' . $matches[0]);
  }
}

/**
 * No tacos
 */
function bot_fourkitchens_no_tacos($to, $data, $matches) {
  $things = bot_fourkitchens_listener_data();
  bot_message($to, $data->nick . ': ' . $things[rand(0, count($things) - 1)]);
}

/**
 * Tacos
 */
function bot_fourkitchens_tacos($to, $data, $matches) {
  $things = bot_fourkitchens_listener_data();
  bot_message($to, $data->nick . ': ' . $things[rand(0, count($things) - 1)]);
}

/**
 * Breakfast tacos
 */
function bot_fourkitchens_breakfast($to, $data, $matches) {
  $things = bot_fourkitchens_listener_data();
  bot_message($to, $data->nick . ': ' . $things[rand(0, count($things) - 1)]);
}

/**
 * Beer
 */
function bot_fourkitchens_beer($to, $data, $matches) {
  $things = bot_fourkitchens_listener_data();
  bot_message($to, $data->nick . ': ' . $things[rand(0, count($things) - 1)]);
}

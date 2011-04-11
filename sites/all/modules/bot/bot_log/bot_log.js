(function($) {

Drupal.behaviors.infinite_log = function() {
  var table = $('#bot-log-day:not(.infinite-log-processed)')
    .addClass('infinite-log-processed');
  
  if (table.length) {
    setInterval(function() {
      var channel = $('#edit-bot-log-channel').val(),
          last_message = table.find('tr[class*="bot-log-nick"]:last'),
          last_message_id = last_message.find('.bot-log-time a').attr('id').substring(1),
          date = $('#edit-bot-log-date-year').val() + '-' + 
            $('#edit-bot-log-date-month').val() + '-' + 
            $('#edit-bot-log-date-day').val();
      
      $.get('/bot/log/ajax/' + channel + '/' + date + '/' + last_message_id, function(data) {
        var rows = $(data).find('tr[class*="bot-log-nick"]');
        table.append(rows);
        var offsettop = parseInt($("body").css("height"));
        window.scrollTo(0, offsettop);
      });
    }, 3000);
  }

};

})(jQuery);

(function ($) {
  jQuery(document).ready(function(){
    var win;
    var checkConnect;
    var $connect = $("#meetup-connect");
    var path = Drupal.settings.basepath;
    var oAuthURL = path + 'meetup/redirect';
    $connect.click(function() {
      win = window.open(oAuthURL, "name", "scrollbars=0,width=972,height=660");
    });
      var linl = window.parent.location.href;
      window.opener.location.href = linl;
       window.location.reload(true);
      parent.close();
      checkConnect = setInterval(function() {
      if (!win || !win.closed) {
        return;
      }
      clearInterval(checkConnect);
      //window.location.reload();
    }, 100);

  });
}(jQuery));

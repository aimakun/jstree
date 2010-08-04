var jQ = jQuery.noConflict();
jQ(document).ready(function (e) {
  jQ.get('json?type=tree', function(response) {
    data = jQ.parseJSON(response);
    jQ('#show').jstree({
      'json_data' : {
        'data' : data,
        'ajax' : {
          'url' : 'json',
          'data' : function (n) {
            return { 'key' : n.attr('id'),'type':'tree' };
          }
        }
      },
      'plugins' : [ 'themes', 'json_data','search' ,'ui']
    });
    
  jQ('#show li a').each( function(index) {
    var link = $(this).parent().attr('link');
    if (link !== undefined) {
      $(this).attr('href', link);
      $(this).click( function(e) {
        e.preventDefault();
        var target = ($(this).attr('href')).split('/');
        target = target[target.length - 1];
        $.get(document.location.href + '/node/' + target, function(data) {
          $('#jstree_content').html(data);
        });
      });
    }
  });

    jQ('#btn').click(function(){
       jQ('#show').jstree("search",jQ('#search_input').val());
     }

   );
 
 });

});

$ = jQuery;

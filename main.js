var jQ = jQuery.noConflict();
jQ(document).ready(function (e) {
  jQ.get('json?type=tree', function(response) {
    data = jQ.parseJSON(response);
    jQ('#jstree_show').jstree({
      'json_data' : {
        'data' : data,
        'ajax' : {
          'url' : 'json',
          'data' : function (n) {
            return { 'key' : n.attr('id'),'type':'tree' };
          }
        }
      },
      'search' : { 'case_insensitive' : true },
      'plugins' : [ 'themes', 'json_data','search' ,'ui']
    });
  
  var current_item = '';
  
  jQ('#jstree_show li a').each( function(index) {
    var link = $(this).parent().attr('link');
    if (link !== undefined) {
      $(this).attr('href', link);
      $(this).click( function(e) {
        e.preventDefault();
        var target = ($(this).attr('href')).split('/');
        var last_index = target.length -1;
        
        if (current_item != target[last_index]) {
          target = current_item = target[last_index];
          $.get(document.location.href + '/node/' + target, function(data) {
            $('#jstree_content').html(data);
            $('#jstree_thumb').html($('#image-items-hidden').html());
            $('#image-items-hidden').detach('');
            if ($('#jstree_thumb').html().length > 0) {
              Lightbox.initList();
            }
          });
        }
      });
    }
  });

    jQ('#btn').click(function(e){
       e.preventDefault();
       jQ('#jstree_show').jstree("search",jQ('#search_input').val());
     }

   );
 
 });

});

$ = jQuery;

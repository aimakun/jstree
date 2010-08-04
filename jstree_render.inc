<?php
function jstree_render(){
  drupal_add_js(drupal_get_path('module', 'jstree') . '/_lib/jquery.js');
  drupal_add_js(drupal_get_path('module', 'jstree') . '/_lib/jquery.cookie.js');
  drupal_add_js(drupal_get_path('module', 'jstree') . '/_lib/jquery.hotkeys.js');
  drupal_add_js(drupal_get_path('module', 'jstree') . '/jquery.jstree.js');
  drupal_add_js(drupal_get_path('module', 'jstree') . '/main.js');
  return '<div id="container">
  <div>
    <input id="search_input" type="text" />
    <input id="btn" type="button" value="Search"/>
  </div>
  <div id="show" style="width:150px;height:200px;float:left;">
   
  </div>
  <div id="jstree_content" style="float:left;"> 
  asd
  </div>
</div>
';
}

function jstree_node($node) {
  $output = '';
  $output =$node->body;
  if (function_exists('imagefield_elements')) {
    // Get imagefield field, only first one.
    $row = db_fetch_object(db_query("SELECT field_name FROM {content_node_field_instance} WHERE type_name='book' AND widget_module='imagefield' AND widget_active=1 LIMIT 1"));
    
    $field_name = $row->field_name;
    
    if (!empty($field_name)) {
      $res = db_query("SELECT 
        imgfield." . $field_name . "_data data, f.filepath 
        FROM {content_" . $field_name . "} imgfield, {files} f 
        WHERE vid=" . $node->vid . " AND imgfield." . $field_name . "_fid=f.fid 
        AND f.status=1 AND " . $field_name . "_list=1 
        ORDER BY delta");
      
      $img_output = '';
      while ($row = db_fetch_object($res)) {
        $data = unserialize($row->data);
        $attributes = array('class' => 'image-item');
        $img_output .= l(theme('imagecache', variable_get('jstree_imagecache_preset', ''), $row->filepath, $data['alt'], $data['title'], $attributes),
          $row->filepath, 
          array('html' => TRUE, 'attributes'=>array('rel' => 'lightbox')));
        //$img_output .= '<img src="' . $row->filepath . '" />';
      }
      
      $output .= '<div class="image-items">' . $img_output . '</div>';
      // Refresh lightbox handler
      $output .= '<script type="text/javascript">Lightbox.initList() ;</script>';
    }
  }
  die($output);
}

function getTreeData($tree) {
  $data = array();
  foreach ($tree as $node_item) {
    $item = array();
    $item['data'] = $node_item['link']['title'];
    $item['attr']['id'] = 'book-node-' . $node_item['link']['nid'];
    $item['attr']['link'] = url('node/' . $node_item['link']['nid']);
    if (!empty($node_item['below']) && is_array($node_item['below'])) {
      $item['children'] = getTreeData($node_item['below']);
    }
    $data[] = $item;
  }
  return $data;
}

function json(){
if($_GET['type']=='tree'){

  $books = book_get_books();
  $ans = array();
  
  foreach ($books as $book) {
    // Load menu tree in each book.
    $tree = book_menu_subtree_data($book);
    //print_dbg($tree);
    $ans[] = getTreeData($tree);
  }
}else{
  $ans=array();
}

return drupal_json($ans);
}

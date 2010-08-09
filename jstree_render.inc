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
    ' . t('Select item on the left side to view descriptions.') . '
  </div>
</div>
';
}

function jstree_node($node) {
  global $user;
  $output = '';
  
  // Get node mlid from book
  $node_book = db_fetch_object(db_query("SELECT mlid FROM {book} WHERE nid=" . $node->nid));
  
  if (!empty($node_book)) {
    if (user_access('create book content')) {
      $output .= '
  <div id="jstree_menu">
      ' . l(t('Add new child on <em>%title</em>', array('%title' => $node->title)), 'node/add/book', 
      array('html' => TRUE, 'query' => array(
        'parent'=>$node_book->mlid, 
        'destination' => 'jstree', // Refer to this module path
      ))) . '
  </div>';
    }
    
    $output .= '  <div class="node-body">' . "\n";
    if ((user_access('edit own book content') && $node->uid == $user->uid) || user_access('edit any book content')) {
      $output .= '    <div class="edit-book">' . l(t('Edit this content'), 'node/' . $node->nid . '/edit', array('query' => array('destination' => 'jstree'))) . "</div>\n";
    }
    $output .= $node->body . '
  </div>';
    
    if (function_exists('imagefield_elements')) {
      // Get imagefield field, only first one.
      $row = db_fetch_object(db_query("SELECT field_name FROM {content_node_field_instance} WHERE type_name='book' AND widget_module='imagefield' AND widget_active=1 LIMIT 1"));
      
      $field_name = $row->field_name;
      
      if (!empty($field_name)) {
        // Check multiple values field or not
        // 1 : multiple = 0, unlimit : multiple = 1, otherwise : multiple = N
        $field_settings = db_fetch_object(db_query("SELECT multiple FROM {content_node_field} WHERE field_name='{$field_name}'"));
        if ($field_settings->multiple > 0) {
          $field_table = "content_{$field_name}";
          $delta = " ORDER BY delta";
        }
        else {
          $field_table = "content_type_book";
          $delta = '';
        }
        
        $res = db_query("SELECT 
          imgfield." . $field_name . "_data data, f.filepath 
          FROM {" . $field_table . "} imgfield, {files} f 
          WHERE vid=" . $node->vid . " AND imgfield." . $field_name . "_fid=f.fid 
          AND f.status=1 AND " . $field_name . "_list=1 
          $delta");
        
        $img_output = '';
        while ($row = db_fetch_object($res)) {
          $data = unserialize($row->data);
          $attributes = 'image-item';
          $img_output .= ' ' . l(theme('imagecache', variable_get('jstree_imagecache_preset', ''), $row->filepath, $data['alt'], $data['title'], $attributes),
            $row->filepath, 
            array('html' => TRUE, 'attributes'=>array('rel' => 'lightbox')));
          //$img_output .= '<img src="' . $row->filepath . '" />';
        }
        
        $output .= '  <div class="image-items">' . $img_output . '</div>';
        // Refresh lightbox handler
        $output .= '  <script type="text/javascript">Lightbox.initList() ; var count=' . rand() . ';</script>';
      }
    }
  }
  
  // Cache results for delay some response (AJAX only, drupal should be cache normal processed page).
  // Note: Aug 4, 2010 - Only Google Chrome couldn't cache this response properly.
  $expires = 60;
  header("Pragma: public");
  header("Cache-Control: maxage=".$expires);
  header('Expires: ' . gmdate('D, d M Y H:i:s', time()+$expires) . ' GMT');
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

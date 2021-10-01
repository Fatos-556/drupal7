<?php

/**
 * @file
 * The include file which controls the Image field.
 */

/**
 * Implements hook_field_widget_info().
 */
function sign_widget_field_widget_info() {
  return array(
    'signature_pad' => array(
      'label' => t('Signature field'),
      'field types' => array('image'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'preview_image_style' => 'large',
        'dotSize' => 1,
        'minWidth' => 0.5,
        'maxWidth' => 2.5,
        'backgroundColor' => 'rgba(0,0,0,0)',
        'penColor' => 'rgba(0,0,0,1)',
        'velocityFilterWeight' => 0.7,
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function sign_widget_field_widget_settings_form($field, $instance) {
  // Use the mfw file widget settings form.
  $form = multiupload_filefield_widget_field_widget_settings_form($field, $instance);
  $widget = $instance['widget'];
  //load config default
  $settings = $widget['settings'];

  $form['dotSize'] = array(
    '#type' => 'textfield',
    '#title' => t('Dot Size'),
    '#default_value' => $settings['dotSize'],
    '#element_validate' => array('element_validate_number'),
    '#required' => TRUE,
    '#size' => 5,
    '#description' => t('Radius of a single dot.'),
  );

  $form['minWidth'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum Width'),
    '#default_value' => $settings['minWidth'],
    '#element_validate' => array('element_validate_number'),
    '#required' => TRUE,
    '#size' => 5,
    '#description' => t('Minimum width of a line'),
  );

  $form['maxWidth'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum Width'),
    '#default_value' => $settings['maxWidth'],
    '#element_validate' => array('element_validate_number'),
    '#required' => TRUE,
    '#size' => 5,
    '#description' => t('Maximum width of a line.'),
  );

  $form['backgroundColor'] = array(
    '#type' => 'textfield',
    '#title' => t('Background Color'),
    '#default_value' => $settings['backgroundColor'],
    '#required' => TRUE,
    '#size' => 15,
    '#description' => t('Color used to clear the background. Use the format rgba(#,#,#,#).'),
  );

  $form['penColor'] = array(
    '#type' => 'textfield',
    '#title' => t('Pen Color'),
    '#default_value' => $settings['penColor'],
    '#required' => TRUE,
    '#size' => 15,
    '#description' => t('Color used to draw the lines. Use the format rgba(#,#,#,#).'),
  );

  $form['velocityFilterWeight'] = array(
    '#type' => 'textfield',
    '#title' => t('Velocity Filter Weight'),
    '#default_value' => $settings['velocityFilterWeight'],
    '#element_validate' => array('element_validate_number'),
    '#required' => TRUE,
    '#size' => 5,
    '#description' => t('Weight used to modify new velocity based on the previous velocity.'),
  );
  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function sign_widget_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Add display_field setting to field because file_field_widget_form() assumes it is set.
  $field['settings']['display_field'] = 0;

  // Load the items for form rebuilds from the field state as they might not be
  // in $form_state['values'] because of validation limitations. Also, they are
  // only passed in as $items when editing existing entities.
  $field_state = field_form_get_state($element['#field_parents'], $field['field_name'], $langcode, $form_state);
  if (isset($field_state['items'])) {
    $items = $field_state['items'];
  }

  $elements = multiupload_filefield_widget_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  $settings = $instance['settings'];

  foreach (element_children($elements) as $delta) {
    // Add all extra functionality provided by the image widget.
    $elements[$delta]['#process'][] = 'sign_field_widget_process';
  }

  if ($field['cardinality'] == 1) {
    // If there's only one field, return it as delta 0.
    if (empty($elements[0]['#default_value']['fid'])) {
      $elements[0]['#description'] = nl2br( $element['#description']);
    }
  }

  return $elements;
}

/**
 * Implements hook_field_widget_form_alter().
 *
 * @param $element
 * @param $form_state
 * @param $context
 */
function sign_widget_field_widget_form_alter(&$element, &$form_state, $context) {
  // If this is an image field type of instance 'field_image_top'
  if (isset($element['#theme']) && $element['#theme'] == 'sign_field_widget_multiple') {
    // Loop through the element children (there will always be at least one).
    foreach (element_children($element) as $key => $child) {
      // Add the new process function to the element
      $element[$key]['#process'][] = 'sign_field_widget_process';
      $element[$key]['#sign_field_size'] = $element['#sign_field_size'];
    }
  }
}

/**
 * Change the image element.
 *
 * @param $element
 * @param $form_state
 * @param $form
 * @return mixed
 */
function sign_field_widget_process($element, &$form_state, $form = []) {
  // Change type of the "_weight" field to hidden.
  if (isset($element['_weight'])) {
    $weight = &$element['_weight'];
    $weight['#type'] = 'hidden';
    $weight += element_info('hidden');
  }
  if(!empty($element["upload_button"])){
    unset($element["upload_button"]);
  }
  if(!empty($element["upload"])){
    unset($element["upload"]);
  }
  $settings = [];
  $delta = !empty($element["#delta"]) ? $element["#delta"] : 0;
  $items = !empty($form_state['items']) ? $form_state['items'] : [];
  if(empty($element["signature"])){
    $markup = '<div class="signature_panel signature_panel_' . $delta . '" data-signature="' . urlencode(isset($items[$delta]['signature']) ? $items[$delta]['signature'] : '') . '"> <canvas class="signature-pad"></canvas></div>';
    $element['signature'] = array(
      '#prefix' => '<div class="esign_container" data-settings=\'' . json_encode($settings) . '\'>'.$markup,
      '#type' => 'hidden',
      '#attributes' => array(
        'class' => array('signature-storage'),
      ),
      '#suffix'=>'</div>'
    );
    if(!empty($element["#file"]->uri)){

      // Get settings of the field.
      $instance = field_widget_instance($element, $form_state);
      $widget_settings = $instance['widget']['settings'];
      $style = image_style_load($widget_settings['preview_image_style']);

      // Prepare dimensions of the preview image.
      while (!isset($size) && !empty($style['effects'])) {
        $effect = array_pop($style['effects']);
        $effect_data = $effect['data'];
        if (!empty($effect_data['height']) && !empty($effect_data['width'])) {
          $size = array(
            'height' => $effect_data['height'],
            'width' => $effect_data['width'],
          );
        }
      }
      $elements['#signature_size'] = $size;
    }
  }
  $element['#theme'] = 'sign_widget_image';

  // Include necessary libraries.
  $element['#attached']['library'] = array(
    array('sign_widget', 'signature_pad'),
  );

  if(!empty($form_state['input'])){
    $files_save = _sign_widget_image_to_file($element["#upload_location"], $form_state['input'][$element["#field_name"]][$element["#language"]]);
    if($files_save){
      $element["#file"] =  end($files_save);
      $form_state["values"][$element["#field_name"]]["und"][0]["fid"] = $element["#file"]->fid;
      unset($element["#element_validate"]);
    }
  }
  return $element;
}

/**
 * helper save image to file
 * @param string $directory destination to save image
 * @param array $esignatures list of signature
 * @return Result all file save
 */
function _sign_widget_image_to_file($uri, $signatures = array()){
  $dir = drupal_realpath($uri);
  $files_save = FALSE;
  if(empty($signatures)) return FALSE;
  foreach ( $signatures as $index => $signature) {
    $sign = $signature["signature"];
    if (file_prepare_directory($dir, FILE_CREATE_DIRECTORY) && !empty($sign)) {
      $filename = time() . '_' . rand(1000, 9999) . '.png';
      $data_uri = $sign;
      $encoded_image = explode(",", $data_uri)[1];
      $decoded_image = base64_decode($encoded_image);
      //Saves a file to the specified destination and creates a database entry.
      $files_save[$index] = file_save_data($decoded_image, $uri . '/' . $filename, FILE_EXISTS_RENAME);
    }
  }
  return $files_save;
}

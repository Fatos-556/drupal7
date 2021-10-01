<?php

/**
 * @file
 * The theme system, which controls the output of the Umage field.
 */

/**
 * Returns HTML for a group of image upload widgets.
 *
 * @param $variables
 * @return string
 */
function theme_sign_widget_multiple($variables) {
  $element = $variables['element'];

  // Get our list of widgets in order (needed when the form comes back after
  // preview or failed validation).
  $widgets = array();
  foreach (element_children($element) as $key) {
    if (isset($element[$key]['#default_value']['_weight'])) {
      $element[$key]['_weight']['#value'] = $element[$key]['#default_value']['_weight'];
    }
    $widgets[] = &$element[$key];
  }
  usort($widgets, '_field_sort_items_value_helper');

  $rows = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'nice-imagefield-sortable',
        'clearfix',
      ),
    ),
  );

  $attributes_style = NULL;
  if (!empty($element['#sign_field_size'])) {
    foreach ($element['#sign_field_size'] as $attribute => $value) {
      $attributes_style .=  $attribute . ':' . $value . 'px;';
    }

    $rows['#attached']['css'] = array(
      'div.ui-sortable-placeholder { ' . $attributes_style . ' }' => array(
        'type' => 'inline',
      ),
    );
  }

  foreach ($widgets as $key => &$widget) {
    // Save the uploading row for last.
    if ($widget['#file'] == FALSE) {
      $widget['#title'] = $element['#file_upload_title'];
      $widget['#description'] = $element['#file_upload_description'];
      continue;
    }

    // Delay rendering of the "Display" option and the weight selector, so that
    // each can be rendered later in its own column.
    if (isset($element['#display_field']) && $element['#display_field']) {
      hide($widget['display']);
    }
    hide($widget['_weight']);

    // Render everything else together in a column, without the normal wrappers.
    $widget['#theme_wrappers'] = array();
    $information = drupal_render($widget);

    $display = '';
    if (isset($element['#display_field']) && $element['#display_field']) {
      unset($widget['display']['#title']);
      $display = array(
        'data' => render($widget['display']),
        'class' => array('checkbox'),
      );
    }

    $weight = render($widget['_weight']);

    // Arrange the item with all of the rendered elements.
    $row = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('nice-imagefield-element'),
        'style' => $attributes_style,
      ),
    );

    $row['info'] = array(
      '#markup' => $information,
    );

    if (isset($element['#display_field']) && $element['#display_field']) {
      $row['display'] = array(
        '#markup' => $display,
        '#prefix' => '<div class="hidden">',
        '#suffix' => '</div>',
      );
    }

    $row['weight'] = array(
      '#markup' => $weight,
      '#prefix' => '<div class="nice-imagefield-weight">',
      '#suffix' => '</div>',
    );

    $rows[] = $row;
  }

  $output = drupal_render($rows);
  $output .= drupal_render_children($element);
  return $output;
}

/**
 * Returns HTML for an image field widget.
 *
 * @param $variables
 * @return string
 */
function theme_sign_widget_image($variables) {
  $element = $variables['element'];

  $rows = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'signature-sortable',
        'clearfix',
      ),
    ),
  );

  $attributes_style = NULL;
  if (!empty($element['#signature_size'])) {
    foreach ($element['#signature_size'] as $attribute => $value) {
      $attributes_style .=  $attribute . ':' . $value . 'px;';
    }

    $rows['#attached']['css'] = array(
      'div.ui-sortable-placeholder { ' . $attributes_style . ' }' => array(
        'type' => 'inline',
      ),
    );
  }

  $output = drupal_render($rows);

  if(!empty($element["#file"]->uri)){
    $image_entity = file_view($element["#file"]);
    $output .= drupal_render($image_entity);
  }
  $output .= drupal_render_children($element);
  return $output;
}

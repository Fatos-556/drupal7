CONTENTS OF THIS FILE
---------------------
   
 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Maintainers


INTRODUCTION
------------

The Image Drupal 7 module allows you to work with a set of images using the
signature pad feature, but in many cases it is uncomfortable.
This module uses the 'sortable' interaction of jQuery UI to overcome this
problem and presents a widget called 'Nice Multiple' that can be used with
fields of Image Type.
Besides this, the module requires Multiupload Filefield Widget module and
allows selecting multiple files to upload at a time.


REQUIREMENTS
------------
Sign Widget module has following dependencies:

Drupal Core Modules
 * Image

Contributed Modules
 * Libraries API (https://www.drupal.org/project/libraries)

 * Multiupload Filefield Widget
   (https://www.drupal.org/project/multiupload_filefield_widget)

External Libraries
 * jQuery Signature pad plugin (https://github.com/szimek/signature_pad)


INSTALLATION
------------
 * Install as you would normally install a contributed Drupal module.
   See https://www.drupal.org/documentation/install/modules-themes/modules-7
   for further information.

 * Add a Image field to a content type and select the widget 'Signature field'.

 * You might have to increase the default max_input_vars, max_file_uploads,
   post_max_size or max_file_uploads php.ini directives.


CONFIGURATION
-------------
Once you enable this module you will have a new field widget type called "Nice
Multiple" at the Field UI (e.g. admin/structure/types/manage/TYPE/fields).


MAINTAINERS
-----------
Current maintainers:
 * Bao NGUYEN (lazzyvn) - https://www.drupal.org/u/lazzyvn

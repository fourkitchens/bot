<?php
// $Id: api-class-page.tpl.php,v 1.1.2.8 2010/08/22 12:25:17 drumm Exp $

/**
 * @file api-class-page.tpl.php
 * Theme implementation to display a class overview.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the class.
 * - $branch: Object with information about the branch.
 * - $class: Object with information about the class.
 * - $defined: HTML reference to file that defines this class.
 * - $is_admin: True or false.
 * - $logged_in: True or false.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $class object.
 * - $class->title: Display name.
 * - $class->object_type: For this template it will be 'class'.
 * - $class->branch_id: An identifier for the branch.
 * - $class->file_name: The path to the file in the source.
 * - $class->summary: A one-line summary of the object.
 * - $class->code: Escaped source code.
 * - $class->see: HTML index of additional references.
 *
 * @see api_preprocess_api_class_page().
 */
?>
<?php print $documentation ?>

<?php if (!empty($see)) { ?>
  <h3><?php print t('See also') ?></h3>
  <?php print $see ?>
<?php } ?>

<?php if (!empty($implements)) { ?>
  <h3><?php print t('Implmented by') ?></h3>
  <?php print $implements ?>
<?php } ?>

<?php if (!empty($hierarchy)) { ?>
  <h3><?php print t('Hierarchy') ?></h3>
  <?php print $hierarchy ?>
<?php } ?>

<?php print $objects; ?>

<?php print $defined; ?>
<?php print theme('ctools_collapsible', t('View code'), $code, $collapsed = TRUE) ?>

<?php if (!empty($related_topics)) { ?>
  <h3><?php print t('Related topics') ?></h3>
  <?php print $related_topics ?>
<?php } ?>

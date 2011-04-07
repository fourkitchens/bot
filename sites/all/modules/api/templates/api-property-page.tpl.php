<?php
// $Id: api-property-page.tpl.php,v 1.1.2.2 2010/06/01 07:15:47 drumm Exp $

/**
 * @file api-property-page.tpl.php
 * Theme implementation to display a function overview.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the function.
 * - $branch: Object with information about the branch.
 * - $property: Object with information about the function.
 * - $defined: HTML reference to file that defines this function.
 * - $is_admin: True or false.
 * - $logged_in: True or false.
 *
 * Available variables in the $branch object:
 * - $branch->project: The machine name of the branch.
 * - $branch->title: A proper title for the branch.
 * - $branch->directories: The local included directories.
 * - $branch->excluded_directories: The local excluded directories.
 *
 * Available variables in the $property object.
 * - $property->title: Display name.
 * - $property->related_topics: Related information about the function.
 * - $property->object_type: For this template it will be 'function'.
 * - $property->branch_id: An identifier for the branch.
 * - $property->file_name: The path to the file in the source.
 * - $property->summary: A one-line summary of the object.
 * - $property->code: Escaped source code.
 * - $property->see: HTML index of additional references.
 * - $property->var: Type of property.
 *
 * @see api_preprocess_api_property().
 */
?>
<?php print $documentation ?>

<?php if (!empty($var)) { ?>
  <p><strong><?php print t('Type') ?>:</strong> <?php print $var; ?></p>
<?php } ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php print $defined; ?>
<?php print $code; ?>

<?php if (!empty($related_topics)) { ?>
  <h3><?php print t('Related topics') ?></h3>
  <?php print $related_topics ?>
<?php } ?>

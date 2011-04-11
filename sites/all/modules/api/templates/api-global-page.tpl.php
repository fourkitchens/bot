<?php
// $Id: api-global-page.tpl.php,v 1.1.2.5 2010/04/21 23:21:12 drumm Exp $

/**
 * @file api-global-page.tpl.php
 * Theme implementation to display a function overview.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the function.
 * - $branch: Object with information about the branch.
 * - $global: Object with information about the function.
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
 * Available variables in the $global object.
 * - $global->title: Display name.
 * - $global->related_topics: Related information about the function.
 * - $global->object_type: For this template it will be 'function'.
 * - $global->branch_id: An identifier for the branch.
 * - $global->file_name: The path to the file in the source.
 * - $global->summary: A one-line summary of the object.
 * - $global->code: Escaped source code.
 * - $global->see: HTML index of additional references.
 *
 * @see api_preprocess_api_global_page().
 */
?>
<?php print $documentation ?>

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

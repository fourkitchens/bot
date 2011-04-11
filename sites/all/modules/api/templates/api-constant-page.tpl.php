<?php
// $Id: api-constant-page.tpl.php,v 1.2.2.4 2010/04/21 23:21:12 drumm Exp $

/**
 * @file api-constant-page.tpl.php
 * Theme implementation to display a constant overview.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the constant.
 * - $branch: Object with information about the branch.
 * - $constant: Object with information about the constant.
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
 * Available variables in the $constant object.
 * - $constant->title: Display name.
 * - $constant->object_type: For this template it will be 'constant'.
 * - $constant->branch_id: An identifier for the branch.
 * - $constant->file_name: The path to the file in the source.
 * - $constant->summary: A one-line summary of the object.
 * - $constant->code: Escaped source code.
 * - $constant->see: HTML index of additional references.
 *
 * @see api_preprocess_api_constant_page().
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

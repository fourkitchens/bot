<?php
// $Id: api-file-page.tpl.php,v 1.1.2.7 2010/05/29 23:25:27 drumm Exp $

/**
 * @file api-file-page.tpl.php
 * Theme implementation for the summary page of a file.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the file.
 * - $file: Object with information about the file.
 * - $is_admin: True or false.
 * - $logged_in: True or false.
 * - $user: user object.
 *
 * Available variables in the $file object:
 * - $file->title: Display name.
 * - $file->summary: Short summary.
 * - $file->documentation: HTML formatted comments.
 * - $file->code: HTML formatted source code.
 * - $file->objects: Documented objects HTML.
 * - $file->see: Related api objects.
 */
?>
<?php if (!empty($file->version)) { ?>
<p><?php print t('Version') ?> <?php print $file->version ?></p>
<?php } ?>

<?php print $documentation ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php print $objects; ?>

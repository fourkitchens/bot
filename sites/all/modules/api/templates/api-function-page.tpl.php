<?php
// $Id: api-function-page.tpl.php,v 1.5.2.13 2010/06/01 07:15:47 drumm Exp $

/**
 * @file api-function-page.tpl.php
 * Theme implementation to display a function overview.
 *
 * Available variables:
 * - $documentation: Documentation from the comment header of the function.
 * - $branch: Object with information about the branch.
 * - $function: Object with information about the function.
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
 * Available variables in the $function object.
 * - $function->title: Display name.
 * - $function->return: What the function returns.
 * - $function->parameters: The function parameters.
 * - $function->related_topics: Related information about the function.
 * - $function->object_type: For this template it will be 'function'.
 * - $function->branch_id: An identifier for the branch.
 * - $function->file_name: The path to the file in the source.
 * - $function->summary: A one-line summary of the object.
 * - $function->code: Escaped source code.
 * - $function->see: HTML index of additional references.
 * - $function->throws: Paragraph describing possible exceptions.
 *
 * @see api_preprocess_api_function_page().
 */
?>
<table id="api-function-signature">
  <thead>
    <tr><th class="branch"><?php print t('Versions') ?></th><th></th></tr>
  </thead>
  <tbody>
    <?php foreach ($signatures as $branch => $signature) { ?>
      <?php if ($signature['active']) { ?>
        <tr class="active">
          <td class="branch"><?php print $branch ?></td>
          <td class="signature"><code><?php print $signature['signature'] ?></code></td>
        </tr>
      <?php } else if ($signature['status']) { ?>
        <tr>
          <td class="branch"><?php print l($branch, $signature['url']) ?></td>
          <td class="signature"><code><?php print l($signature['signature'], $signature['url'], array('html' => TRUE)) ?></code></td>
        </tr>
      <?php } else { ?>
        <tr>
          <td class="branch"><?php print $branch ?></td>
          <td class="signature"><code><?php print $signature['signature'] ?></code></td>
        </tr>
      <?php } ?>
    <?php } ?>
  </tbody>
</table>

<?php print $documentation ?>

<?php if (!empty($parameters)) { ?>
<h3><?php print t('Parameters') ?></h3>
<?php print $parameters ?>
<?php } ?>

<?php if (!empty($return)) { ?>
<h3><?php print t('Return value') ?></h3>
<?php print $return ?>
<?php } ?>

<?php if (!empty($throws)) { ?>
  <h3><?php print t('Throws') ?></h3>
  <?php print $throws ?>
<?php } ?>

<?php if (!empty($see)) { ?>
<h3><?php print t('See also') ?></h3>
<?php print $see ?>
<?php } ?>

<?php if (!empty($related_topics)) { ?>
<h3><?php print t('Related topics') ?></h3>
<?php print $related_topics ?>
<?php } ?>

<?php print $call ?>

<h3><?php print t('Code'); ?></h3>
<?php print $defined; ?>
<?php print $code; ?>

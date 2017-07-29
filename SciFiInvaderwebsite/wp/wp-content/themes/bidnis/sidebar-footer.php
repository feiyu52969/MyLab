<?php
/**
 * Template for displaying a widget area and additional content in the footer
 *
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
 */

if(
	!is_active_sidebar( 'footer-top-widget-area' ) &&
	!is_active_sidebar( 'footer-one-widget-area' ) &&
	!is_active_sidebar( 'footer-two-widget-area' ) &&
	!is_active_sidebar( 'footer-three-widget-area' ) &&
	!is_active_sidebar( 'footer-four-widget-area' )
){ 
	return;
} ?>

<?php if( is_active_sidebar( 'footer-top-widget-area' ) ): ?>
	<div class="footer-top-widget-area widget-area wrapper" role="complementary">
		<?php dynamic_sidebar( 'footer-top-widget-area' ); ?>
	</div><!-- .footer-top-widget-area .widget-area -->
<?php endif; ?>

<?php if(
	!is_active_sidebar( 'footer-one-widget-area' ) &&
	!is_active_sidebar( 'footer-two-widget-area' ) &&
	!is_active_sidebar( 'footer-three-widget-area' ) &&
	!is_active_sidebar( 'footer-four-widget-area' )
){ 
	return;
} ?>

<div class="footer-widget-areas-wrapper wrapper" role="complementary">

	<?php if( is_active_sidebar( 'footer-one-widget-area' ) ): ?>
		<div class="footer-one-widget-area widget-area" role="complementary">
			<?php dynamic_sidebar( 'footer-one-widget-area' ); ?>
		</div><!-- .footer-one-widget-area .widget-area -->
	<?php endif; ?>

	<?php if( is_active_sidebar( 'footer-two-widget-area' ) ): ?>
		<div class="footer-two-widget-area widget-area" role="complementary">
			<?php dynamic_sidebar( 'footer-two-widget-area' ); ?>
		</div><!-- .footer-two-widget-area .widget-area -->
	<?php endif; ?>

	<?php if( is_active_sidebar( 'footer-three-widget-area' ) ): ?>
		<div class="footer-three-widget-area widget-area" role="complementary">
			<?php dynamic_sidebar( 'footer-three-widget-area' ); ?>
		</div><!-- .footer-three-widget-area .widget-area -->
	<?php endif; ?>

	<?php if( is_active_sidebar( 'footer-four-widget-area' ) ): ?>
		<div class="footer-four-widget-area widget-area" role="complementary">
			<?php dynamic_sidebar( 'footer-four-widget-area' ); ?>
		</div><!-- .footer-four-widget-area .widget-area -->
	<?php endif; ?>

</div>
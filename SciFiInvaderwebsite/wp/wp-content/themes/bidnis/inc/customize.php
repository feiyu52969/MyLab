<?php
/**
	* Bidnis Customize
	*
	* Adds various theme options to...
	*
	* 
 * @package KniffTech
 * @subpackage Bidnis
 * @since Bidnis 1.0
	*/
	function bidnis_customize_register( $wp_customize ){
		// Toggle site title
		$wp_customize->add_setting( 'display_site_title', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'display_site_title', array(
			'label'			=> __( 'Display Site Title', 'bidnis' ),
			'section'		=> 'title_tagline',
			'type'			=> 'checkbox',
		) );
		
		// Toggle site description
		$wp_customize->add_setting( 'display_tagline', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'display_tagline', array(
			'label'			=> __( 'Display Tagline', 'bidnis' ),
			'section'		=> 'title_tagline',
			'type'			=> 'checkbox',
		) );

		// Color scheme
		$wp_customize->add_setting( 'color_scheme', array(
			'default'	=> '#ffd200',
			'sanitize_callback' => 'sanitize_hex_color'
		) );

		$wp_customize->add_control( new WP_Customize_Color_Control( $wp_customize, 'color_scheme', array(
			'label'        => __( 'Color scheme', 'bidnis' ),
			'section'    => 'colors',
			'settings'   => 'color_scheme',
		) ) );

		// Header text color
		$wp_customize->add_setting( 'header-text-color', array(
			'default'	=> '#191919',
			'sanitize_callback' => 'sanitize_hex_color'
		) );

		$wp_customize->add_control( new WP_Customize_Color_Control( $wp_customize, 'header-text-color', array(
			'label'        => __( 'Header text color', 'bidnis' ),
			'section'    => 'colors',
			'settings'   => 'header-text-color',
		) ) );

		// Text color
		$wp_customize->add_setting( 'text_color', array(
			'default'	=> '#191919',
			'sanitize_callback' => 'sanitize_hex_color'
		) );

		$wp_customize->add_control( new WP_Customize_Color_Control( $wp_customize, 'text_color', array(
			'label'        => __( 'Text Color', 'bidnis' ),
			'section'    => 'colors',
			'settings'   => 'text_color',
		) ) );

		// Anchor text color
		$wp_customize->add_setting( 'link_color', array(
			'default'	=> '#191919',
			'sanitize_callback' => 'sanitize_hex_color'
		) );

		$wp_customize->add_control( new WP_Customize_Color_Control( $wp_customize, 'link_color', array(
			'label'        => __( 'Link Color', 'bidnis' ),
			'section'    => 'colors',
			'settings'   => 'link_color',
		) ) );

		// Entry meta section
		$wp_customize->add_section( 'entry_meta' , array(
			'title'      => __( 'Entry Metadata', 'bidnis' ),
			'priority'   => 80,
		) );

		// Entry meta author
		$wp_customize->add_setting( 'entry_meta_author', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'entry_meta_author', array(
			'label'			=> __('Display author', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox',
		) );

		// Entry meta date
		$wp_customize->add_setting( 'entry_meta_date', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'entry_meta_date', array(
			'label'			=> __('Display date posted', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox',
		) );

		// Entry meta comments
		$wp_customize->add_setting( 'entry_meta_comments', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'entry_meta_comments', array(
			'label'			=> __('Display comment count', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox',
		) );

		// Entry meta post format
		$wp_customize->add_setting( 'entry_meta_post_format', array( 
			'default' => true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox' 
		) );
	
		$wp_customize->add_control( 'entry_meta_post_format', array(
			'label'			=> __('Display Post Format', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox'
		) );

		// Entry meta categories
		$wp_customize->add_setting( 'entry_meta_categories', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'entry_meta_categories', array(
			'label'			=> __('Display Categories', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox',
		) );

		// Entry meta tags
		$wp_customize->add_setting( 'entry_meta_tags', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'entry_meta_tags', array(
			'label'			=> __('Display Tags', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox',
		) );

		// author bio
		$wp_customize->add_setting( 'author_bio', array( 
			'default' => true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'author_bio', array(
			'label'			=> __('Display Author Bio', 'bidnis'),
			'section'		=> 'entry_meta',
			'type'			=> 'checkbox'
		) );

		// Theme options
		$wp_customize->add_section( 'design_options' , array(
			'title'      => __( 'Theme options', 'bidnis' ),
			'priority'   => 90,
		) );

		// Breadcrumbs
		$wp_customize->add_setting( 'breadcrumbs', array(
			'default'	=> false,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'breadcrumbs', array(
			'label'			=> __('Display breadcrumbs', 'bidnis'),
			'section'		=> 'design_options',
			'type'			=> 'checkbox',
		) );

		// Display full content or summery/excerpt
		$wp_customize->add_setting( 'display_content', array(
			'default'	=> false,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'display_content', array(
			'label'			=> __('Display full content of posts on index pages', 'bidnis'),
			'section'		=> 'design_options',
			'type'			=> 'checkbox',
		) );

		// Display thumbnails on index pages
		$wp_customize->add_setting( 'thumbnail_index', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'thumbnail_index', array(
			'label'			=> __('Display thumbnails on index pages', 'bidnis'),
			'section'		=> 'design_options',
			'type'			=> 'checkbox',
		) );

		// Display thumbnails on content pages
		$wp_customize->add_setting( 'thumbnail_content', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'thumbnail_content', array(
			'label'			=> __('Display thumbnails on content pages', 'bidnis'),
			'section'		=> 'design_options',
			'type'			=> 'checkbox',
		) );

		// Display read more button
		$wp_customize->add_setting( 'read_more', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'read_more', array(
			'label'			=> __('Display read more button', 'bidnis'),
			'section'		=> 'design_options',
			'type'			=> 'checkbox',
		) );

		// Page numbers on index pages
		$wp_customize->add_setting( 'page_numbers', array(
			'default'	=> false,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'page_numbers', array(
			'label'			=> __('Display page numbers on index pages', 'bidnis'),
			'section'		=> 'design_options',
			'type'			=> 'checkbox',
		) );

		// Header Image text
		$wp_customize->add_setting( 'header_image_text', array(
			'default'	=> '',
			'sanitize_callback' => 'sanitize_text_field'
		) );

		$wp_customize->add_control( 'header_image_text', array(
			'label'			=> __('Text', 'bidnis'),
			'section'		=> 'header_image',
			'type'			=> 'text',
		) );

		// Header Image cta text
		$wp_customize->add_setting( 'header_image_cta_text', array(
			'default'	=> '',
			'sanitize_callback' => 'sanitize_text_field'
		) );

		$wp_customize->add_control( 'header_image_cta_text', array(
			'label'			=> __('Button text', 'bidnis'),
			'section'		=> 'header_image',
			'type'			=> 'text',
		) );

		// Header Image cta url
		$wp_customize->add_setting( 'header_image_cta_url', array(
			'default'	=> '',
			'sanitize_callback' => 'sanitize_text_field'
		) );

		$wp_customize->add_control( 'header_image_cta_url', array(
			'label'			=> __('Button URL (link)', 'bidnis'),
			'section'		=> 'header_image',
			'type'			=> 'text',
		) );


		// Social icons
		$wp_customize->add_section( 'social_icons' , array(
			'title'      => __( 'Contact and Social Media', 'bidnis' ),
			'priority'   => 60,
		) );

		// RSS link
		$wp_customize->add_setting( 'social_media_rss', array(
			'default'	=> false,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'social_media_rss', array(
			'label'			=> __('RSS Link', 'bidnis'),
			'section'		=> 'social_icons',
			'type'			=> 'checkbox',
		) );

		// Phone number
		$wp_customize->add_setting( 'phone', array(
			'default'	=> '',
			'sanitize_callback' => 'sanitize_text_field'
		) );

		$wp_customize->add_control( 'phone', array(
			'label'			=> __('Phone number', 'bidnis'),
			'section'		=> 'social_icons',
			'type'			=> 'text',
		) );

		// E-mail
		$wp_customize->add_setting( 'email', array(
			'default'	=> '',
			'sanitize_callback' => 'sanitize_text_field'
		) );

		$wp_customize->add_control( 'email', array(
			'label'			=> __('E-mail', 'bidnis'),
			'section'		=> 'social_icons',
			'type'			=> 'text',
		) );

		// Social icons
		global $bidnis_social_media_icons;

		foreach( $bidnis_social_media_icons as $service => $icon){
			$wp_customize->add_setting( 'social_media_'.strtolower($service), array(
				'default'	=> '',
				'sanitize_callback' => 'sanitize_text_field'
			) );
	
			$wp_customize->add_control( 'social_media_'.strtolower($service), array(
				'label'			=> $service.' URL',
				'section'		=> 'social_icons',
				'type'			=> 'text',
			) );
		}

		// Footer section
		$wp_customize->add_section( 'footer' , array(
			'title'      => __( 'Footer', 'bidnis' ),
			'priority'   =>	120,
		) );

		// Change copyright text
		$wp_customize->add_setting( 'footer_text', array(
			'default'	=> get_bloginfo('name'),
			'sanitize_callback' => 'sanitize_text_field'
		) );

		$wp_customize->add_control( 'footer_text', array(
			'label'			=> __('Footer text', 'bidnis'),
			'section'		=> 'footer',
			'type'			=> 'text',
		) );

		// Toggle copyright icon
		$wp_customize->add_setting( 'footer_copyright', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'footer_copyright', array(
			'label'			=> __('Show Copyright Icon', 'bidnis'),
			'section'		=> 'footer',
			'type'			=> 'checkbox',
		) );

		// Toggle current year
		$wp_customize->add_setting( 'footer_year', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'footer_year', array(
			'label'			=> __('Show Current Year', 'bidnis'),
			'section'		=> 'footer',
			'type'			=> 'checkbox',
		) );

		// Advertise theme author
		$wp_customize->add_setting( 'footer_advert', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'footer_advert', array(
			'label'			=> __('Advertise Theme', 'bidnis'),
			'section'		=> 'footer',
			'type'			=> 'checkbox',
		) );

		// Scroll to top icon
		$wp_customize->add_setting( 'scrolltotop', array(
			'default'	=> true,
			'sanitize_callback' => 'bidnis_sanitize_checkbox'
		) );
	
		$wp_customize->add_control( 'scrolltotop', array(
			'label'			=> __('Scroll to top icon in footer', 'bidnis'),
			'section'		=> 'footer',
			'type'			=> 'checkbox',
		) );
	}
	add_action('customize_register', 'bidnis_customize_register');

	function bidnis_sanitize_checkbox( $input ) {
		// Boolean check 
		return ( ( isset( $input ) && true == $input ) ? true : false );
	}
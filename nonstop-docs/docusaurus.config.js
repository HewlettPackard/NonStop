// @ts-check
// Note: type annotations allow type checking and IDEs autocompletion

//NonStop is veteran platform - the NonStop upgrades world's IT services by handling the burden of clustering and load balancing.

const lightCodeTheme = require('prism-react-renderer/themes/github');
const darkCodeTheme = require('prism-react-renderer/themes/dracula');

/** @type {import('@docusaurus/types').Config} */
const config = {
  title: 'HPE NonStop Developer Portal',
  tagline: "NonStop upgrades the world's IT services by removing the need for clustering and load balancing.",
  url: 'https://your-docusaurus-test-site.com',
  baseUrl: '/',
  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'warn',
  favicon: 'img/element.svg',

  // GitHub pages deployment config.
  // If you aren't using GitHub pages, you don't need these.
  organizationName: 'hpe', // Usually your GitHub org/user name.
  projectName: 'nonstop-docs', // Usually your repo name.

  // Even if you don't use internalization, you can use this field to set useful
  // metadata like html lang. For example, if your site is Chinese, you may want
  // to replace "en" with "zh-Hans".
  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      /** @type {import('@docusaurus/preset-classic').Options} */
      ({
        docs: {
          sidebarPath: require.resolve('./sidebars.js'),
          // Please change this to your repo.
          // Remove this to remove the "edit this page" links.
          editUrl:
            'https://github.com/facebook/docusaurus/tree/main/packages/create-docusaurus/templates/shared/',
        },
        blog: {
          showReadingTime: true,
          // Please change this to your repo.
          // Remove this to remove the "edit this page" links.
          editUrl:
            'https://developer.hpe.com/platform/hpe-nonstop/home/',
        },
        theme: {
          customCss: require.resolve('./src/css/custom.css'),
        },
      }),
    ],
  ],

  themeConfig:
    /** @type {import('@docusaurus/preset-classic').ThemeConfig} */
    ({
      navbar: {
        title: 'HPE NonStop Developer Portal',
        logo: {
          alt: 'HPE Logo',
          src: 'img/element.svg',
        },
        items: [
          // {
          //   type: 'doc',
          //   docId: '/products/overview',
          //   position: 'left',
          //   label: 'Products',
          // },
          {to: '/docs/category/the-platform', label: 'Platform', position: 'left'},
          {to: '/docs/category/our-database', label: 'Database', position: 'left'},
          
          // software and security?
          {to: '/docs/category/our-software-portfolio', label: 'Software', position: 'left'},
          
          {to: '/docs/category/the-middleware', label: 'Middleware', position: 'left'},
          {to: '/docs/category/hpe-nonstops-greenlake', label: 'GreenLake', position: 'left'},
          //{to: '/docs/category/introduction---to-nonstop-os', label: 'Quick Start', position: 'left'},
          //{to: 'https://developer.hpe.com/platform/hpe-nonstop/home/', label: 'HPE Dev', position: 'left'},
          
          
          //{to: 'https://developer.hpe.com/products/nonstop', label: 'Dev Landing Page', position: 'left'},
          {
            href: 'https://github.com/facebook/docusaurus',
            label: 'GitHub - unedited link',
            position: 'right',
          },
        ],
      },
      footer: {
        style: 'dark',
        links: [
          {
            title: 'Docs',
            items: [
              {
                label: 'Quick Start',
                to: '/docs/category/introduction---to-nonstop-os',
              },
            ],
          },
          {
            title: 'Community - unedited links',
            items: [
              {
                label: 'Stack Overflow',
                href: 'https://stackoverflow.com/questions/tagged/docusaurus',
              },
              {
                label: 'Discord',
                href: 'https://discordapp.com/invite/docusaurus',
              },
              {
                label: 'Twitter',
                href: 'https://twitter.com/docusaurus',
              },
            ],
          },
          {
            title: 'More',
            items: [
              {
                label: 'HPE Dev',
                to: 'https://developer.hpe.com/platform/hpe-nonstop/home/',
              },
              {
                label: 'GitHub - unedited link',
                href: 'https://github.com/facebook/docusaurus',
              },
            ],
          },
        ],
        copyright: `Copyright © ${new Date().getFullYear()} HPE NonStop Developer Portal. Built with Docusaurus.`,
      },
      prism: {
        theme: lightCodeTheme,
        darkTheme: darkCodeTheme,
      },
    }),
};

module.exports = config;

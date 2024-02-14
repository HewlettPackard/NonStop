import React from 'react';
import clsx from 'clsx';
import styles from './styles.module.css';

const FeatureList = [
  {
    //onboarding platform
    title: 'Some First Steps',
    Svg: require('@site/static/img/undraw_docusaurus_mountain.svg').default,
    description: (
      <>
        Our NonStop Developer Portal shares tech tips to get developers and engineers onboarded and hit the ground running. Just some quick starts to get the baby-walker things out of the way.
      </>
    ),
  },
  {
    //developer community driven
    title: 'Calling NonStop Developers',
    Svg: require('@site/static/img/undraw_docusaurus_tree.svg').default,
    description: (
      <>
       Check our GitHub for inspirational repos and non-copyrighted code. Share ideas and frustration with the rest of our NonStop community - we're doing this in Tandem!
      </>
    ),
  },
  {
    //inspire product roadmaps
    title: 'Help Me Please!',
    Svg: require('@site/static/img/undraw_docusaurus_react.svg').default,
    description: (
      <>
        Hate getting stuck? Us too. Drop us a mail, and let's find a workaround with some NonStop veterans.
      </>
    ),
  },
];

function Feature({Svg, title, description}) {
  return (
    <div className={clsx('col col--4')}>
      <div className="text--center">
        <Svg className={styles.featureSvg} role="img" />
      </div>
      <div className="text--center padding-horiz--md">
        <h3>{title}</h3>
        <p>{description}</p>
      </div>
    </div>
  );
}

export default function HomepageFeatures() {
  return (
    <section className={styles.features}>
      <div className="container">
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}

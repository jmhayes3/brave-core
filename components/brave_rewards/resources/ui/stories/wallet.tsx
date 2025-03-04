/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import { storiesOf } from '@storybook/react'
import { withKnobs, object, text, select, boolean, number } from '@storybook/addon-knobs'
import centered from '@storybook/addon-centered'

// Components
import { WalletSummary, WalletEmpty, WalletOff, WalletPanel, WalletSummarySlider, WalletWrapper } from '../components'
import { AlertWallet, Notification, WalletState } from '../components/walletWrapper'
import { WalletAddIcon, WalletImportIcon } from 'brave-ui/components/icons'
import { WalletInfoHeader } from '../components/mobile'

const favicon = require('./img/brave-favicon.png')

const doNothing = () => {
  console.log('nothing')
}

storiesOf('Rewards/Wallet/Desktop', module)
  .addDecorator(withKnobs)
  .addDecorator(centered)
  .add('Wrapper', () => {
    const onEnableAds = () => {
      console.log('enabling ads')
    }
    const alert: AlertWallet = {
      node: 'Some text',
      type: 'success',
      onAlertClose: doNothing
    }
    const adsLaunchNotification: Notification = {
      id: '001',
      type: 'ads-launch',
      onCloseNotification: doNothing,
      text: <span>Now you can earn by viewing ads.</span>
    }
    const showAlert = boolean('Show alert', false)
    const showGrant = boolean('Show grants', false)

    const state: WalletState = select('wallet status', {
      unverified: 'unverified',
      verified: 'verified',
      'disconnected unverified': 'disconnected_unverified',
      'disconnected verified': 'disconnected_verified'
    }, 'unverified') as WalletState

    return (
      <WalletWrapper
        compact={false}
        contentPadding={false}
        onNotificationClick={onEnableAds}
        notification={boolean('Show notification', true) ? adsLaunchNotification : undefined}
        showCopy={boolean('Show Uphold', false)}
        showSecActions={boolean('Show secondary actions', true)}
        balance={text('Balance', '25.0')}
        converted={text('Converted', '163230.50 USD')}
        actions={[
          {
            name: 'Add funds',
            action: doNothing,
            icon: <WalletAddIcon />
          },
          {
            name: 'Withdraw Funds',
            action: doNothing,
            icon: <WalletImportIcon />
          }
        ]}
        grants={showGrant ? [
          {
            tokens: '8.0',
            expireDate: '7/15/2018',
            type: 'ugp'
          },
          {
            tokens: '10.0',
            expireDate: '9/10/2018',
            type: 'ugp'
          },
          {
            tokens: '10.0',
            expireDate: '10/10/2018',
            type: 'ads'
          }
        ] : []}
        alert={showAlert ? alert : undefined}
        walletState={state}
        onDisconnectClick={doNothing}
        userName={'Brave Software'}
      >
        Some content
      </WalletWrapper>
    )
  })
  .add('Empty', () => {
    return (
      <div style={{ width: '373px', background: '#f9fbfc', padding: '0 25px' }}>
        <WalletEmpty />
      </div>
    )
  })
  .add('Summary', () => {
    return (
      <div style={{ width: '373px', background: '#f9fbfc', padding: '0 25px' }}>
        <WalletSummary
          report={{
            grant: object('Grant', { tokens: '10.0', converted: '0.25' }),
            deposit: object('Deposit', { tokens: '10.0', converted: '0.25' }),
            ads: object('Ads', { tokens: '10.0', converted: '0.25' }),
            contribute: object('Contribute', { tokens: '10.0', converted: '0.25' }),
            donation: object('Donation', { tokens: '2.0', converted: '0.25' }),
            tips: object('Tips', { tokens: '19.0', converted: '5.25' })
          }}
          onActivity={doNothing}
          reservedAmount={number('Reserved amount', 52)}
          reservedMoreLink={'https://brave.com'}
          onSeeAllReserved={doNothing}
        />
      </div>
    )
  })
  .add('Panel', () => {
    return (
      <div style={{ width: '373px', background: '#fff' }}>
        <WalletPanel
          id={'wallet-panel'}
          platform={select<any>('Provider', { youtube: 'youtube', twitter: 'twitter', twitch: 'twitch', reddit: 'reddit', vimeo: 'vimeo', github: 'github' }, 'youtube')}
          publisherImg={favicon}
          publisherName={'Jonathan Doe'}
          monthlyAmount={'10.0'}
          isVerified={boolean('Verified', true)}
          tipsEnabled={boolean('Tips Enabled', true)}
          includeInAuto={boolean('Include in monthly', true)}
          acEnabled={boolean('AC enabled?', true)}
          attentionScore={'15'}
          donationAmounts={
            [
              {
                tokens: '0.0',
                converted: '0.00'
              },
              {
                tokens: '1.0',
                converted: '0.50'
              },
              {
                tokens: '5.0',
                converted: '2.50'
              },
              {
                tokens: '10.0',
                converted: '5.00'
              }
            ]
          }
          onToggleTips={doNothing}
          donationAction={doNothing}
          onAmountChange={doNothing}
          onIncludeInAuto={doNothing}
          showUnVerified={boolean('Show unverified content', true)}
          onRefreshPublisher={doNothing}
        />
      </div>
    )
  })
  .add('Off', () => {
    return (
      <div style={{ width: '373px', background: '#f9fbfc', padding: '0 25px' }}>
        <WalletOff />
      </div>
    )
  })
  .add('Summary Slider', () => {
    return (
      <div style={{ width: '373px', padding: '0 25px' }}>
        <WalletSummarySlider
          id={'summary-slider'}
        />
      </div>
    )
  })
storiesOf('Rewards/Wallet/Mobile', module)
  .addDecorator(withKnobs)
  .addDecorator(centered)
  .add('Wallet Info Header', () => {
    return (
      <div style={{ position: 'fixed', top: 0, left: 0, height: '100vh', width: '100%' }}>
        <div style={{ width: '100%', position: 'absolute', top: '30%' }}>
          <WalletInfoHeader
            id={'info-header'}
            onClick={doNothing}
            balance={text('Balance', '30.0')}
            converted={text('Converted', '163230.50 USD')}
          />
        </div>
      </div>
    )
  })

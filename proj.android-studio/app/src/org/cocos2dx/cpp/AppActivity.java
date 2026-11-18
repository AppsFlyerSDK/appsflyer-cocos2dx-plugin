/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import android.content.Intent;
import android.os.Bundle;

import com.appsflyer.cocos2dx.rpc.AppsFlyerCocosRpcBridge;

import org.cocos2dx.lib.Cocos2dxActivity;

public class AppActivity extends Cocos2dxActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Before Cocos runs native applicationDidFinishLaunching (GL init), RPC needs Activity/app context.
        AppsFlyerCocosRpcBridge.onActivityCreated(this);
        super.onCreate(savedInstanceState);
    }

    /**
     * Deep links that reopen an already-running activity arrive here (e.g. singleTask).
     * Cocos2dxActivity does not call setIntent(), so getIntent() would otherwise stay
     * on the original launch intent. AppsFlyer reads the activity intent around onResume;
     * we only refresh it here and let the native SDK handle UDL — no performDeepLinking
     * from this hook (avoids racing automatic resolution).
     */
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
    }

    @Override
    protected void onDestroy() {
        AppsFlyerCocosRpcBridge.onActivityDestroyed();
        super.onDestroy();
    }
}

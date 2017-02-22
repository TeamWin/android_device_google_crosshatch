/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.qti.editnumber;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.provider.ContactsContract.Contacts;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Toast;
import android.util.Log;

/**
 * This class contains Qti specific Deflect/ECT feature functions
 * for dynamic call number.
 */
public class QtiEditNumberDialog extends Activity {

    private EditText mEditText;
    private Button mOkButton;
    private ImageButton mContactButton;
    private Button mCancelButton;
    private Cursor mCursor;
    private ListView mListView;
    private String mEditNumber;
    private static String LOG_TAG = "QtiEditNumberDialog";
    private static final int ACTIVITY_REQUEST_CONTACT_PICK = 100;
    private static final String INTENT_PICK_ACTION = "android.intent.action.PICK";
    public static final String INTENT_ACTION_DIALOG_DISMISS =
            "com.qti.editnumber.INTENT_ACTION_DIALOG_DISMISS";

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent != null &&
                    intent.getAction().equals(INTENT_ACTION_DIALOG_DISMISS)) {
                finishActivity(ACTIVITY_REQUEST_CONTACT_PICK);
                finish();
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edit_number);
        mEditText = (EditText) findViewById(R.id.edit_number);
        mContactButton = (ImageButton) findViewById(R.id.contactlist);
        mOkButton = (Button) findViewById(R.id.btn_ok);
        mCancelButton = (Button) findViewById(R.id.btn_cancel);

        // register for broadcast
        IntentFilter filter = new IntentFilter();
        filter.addAction(INTENT_ACTION_DIALOG_DISMISS);
        this.registerReceiver(mReceiver, filter);

        // Allow the title to be set to a custom String using an extra on the intent
        String title = getIntent().getStringExtra("Title");
        if (title != null) {
            setTitle(title);
        }
        mOkButton.setEnabled(false);

        mEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                mOkButton.setEnabled(true);
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                if (mEditText.getText().toString().isEmpty()) {
                    mOkButton.setEnabled(false);
                }
            }
        });

        mContactButton.setOnClickListener(new ImageButton.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                Intent intent = new Intent(INTENT_PICK_ACTION, Contacts.CONTENT_URI);
                startActivityForResult(intent,ACTIVITY_REQUEST_CONTACT_PICK);
            }
        });

        mOkButton.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                if (mEditText.getText().toString().isEmpty()) {
                    return;
                }
                mEditNumber = mEditText.getText().toString();
                Log.d(LOG_TAG, "EditText number  mEditNumber = " + mEditNumber);
                finishWithResult(mEditNumber);
            }
        });

        mCancelButton.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                finish();
            }
        });

    }

    /**
     * Return number to called Activity
     */
    private void finishWithResult(String number) {
        Bundle contactData = new Bundle();
        contactData.putString("Number", number);
        Intent intent = new Intent();
        intent.putExtras(contactData);
        setResult(RESULT_OK, intent);
        finish();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (data == null) {
            Log.w(LOG_TAG, "Data is null from intent" );
            return;
        }

        if (resultCode == Activity.RESULT_OK) {
            Uri contactData = data.getData();
            Cursor c = managedQuery(contactData, null, null, null, null);
            if (c.moveToFirst()) {
                final String id = c.getString(c.getColumnIndexOrThrow(
                        ContactsContract.Contacts._ID));
                final String hasPhone =
                        c.getString(c.getColumnIndex(ContactsContract.Contacts.HAS_PHONE_NUMBER));
                String number = "";
                if (hasPhone.equalsIgnoreCase("1")) {
                    Cursor phones = getContentResolver().query(
                            ContactsContract.CommonDataKinds.Phone.CONTENT_URI, null,
                            ContactsContract.CommonDataKinds.Phone.CONTACT_ID + " = " + id,
                            null, null);
                    phones.moveToFirst();
                    number = phones.getString(phones.getColumnIndex("data1"));
                    mEditText.setText(number);
                }
            }
        }
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();
        this.getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
    }

    @Override
    public void onDestroy() {
        if (mReceiver != null) {
            unregisterReceiver(mReceiver);
            mReceiver = null;
        }
        super.onDestroy();
    }
}

// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/shared/edit_device_bottom_button.dart';
import 'package:snoozeless/theme.dart';

// Third party packages
import 'package:font_awesome_flutter/font_awesome_flutter.dart';

class EditAlarmForm extends StatefulWidget {
  final Alarm alarm;

  const EditAlarmForm({Key? key, required this.alarm}) : super(key: key);

  @override
  _EditAlarmFormState createState() => _EditAlarmFormState();
}

class _EditAlarmFormState extends State<EditAlarmForm> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: FirestoreService().getAlarm(widget.alarm),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Text('Loading');
        } else if (snapshot.hasError) {
          return const Text('Error in edit_alarm_form.dart');
        } else if (snapshot.hasData) {
          var initialAlarm = snapshot.data! as Alarm;
          var modifiedAlarm = Alarm();

          return Padding(
            padding: const EdgeInsets.only(
              left: 20,
              right: 20,
              top: 15,
            ),
            child: Form(
              key: _formKey,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  const Text('Alarm Name'),
                  Container(
                    padding: const EdgeInsets.only(bottom: 45),
                    child: TextFormField(
                      initialValue: initialAlarm.alarmName,
                      validator: (value) {
                        if (value == null || value.isEmpty) {
                          return 'Alarm Name cannot be blank.';
                        } else {
                          modifiedAlarm.alarmName = value;
                          return null;
                        }
                      },
                    ),
                  ),
                  const Text('Wakeup Time'),
                  WakeupTimePicker(
                      initialAlarm: initialAlarm, modifiedAlarm: modifiedAlarm),
                  Row(
                    children: [
                      EditDeviceBottomButton(
                        color: Colors.green,
                        icon: FontAwesomeIcons.check,
                        text: 'Save',
                        method: () {
                          if (_formKey.currentState!.validate()) {
                            FirestoreService().updateAlarm(
                              initialAlarm,
                              modifiedAlarm,
                            );
                            Navigator.of(context).pop();
                          }
                        },
                      ),
                      const SizedBox(
                        width: 10,
                      ),
                      EditDeviceBottomButton(
                        color: Colors.red,
                        icon: FontAwesomeIcons.trash,
                        text: 'Delete',
                        method: () {
                          // showDialog(
                          //   context: context,
                          //   builder: (_) => DeleteAlarmDialog(
                          //     Alarm: initialAlarm,
                          //   ),
                          // );
                        },
                      ),
                    ],
                  ),
                ],
              ),
            ),
          );
        } else {
          return const Text('No data found in database.');
        }
      },
    );
  }
}

class WakeupTimePicker extends StatefulWidget {
  const WakeupTimePicker({
    Key? key,
    required this.initialAlarm,
    required this.modifiedAlarm,
  }) : super(key: key);

  final Alarm initialAlarm;
  final Alarm modifiedAlarm;

  @override
  State<WakeupTimePicker> createState() => _WakeupTimePickerState();
}

class _WakeupTimePickerState extends State<WakeupTimePicker> {
  String? _selectedTime;

  Future<void> _show() async {
    final TimeOfDay? selection = await showTimePicker(
      context: context,
      initialTime: widget.initialAlarm.wakeupTimeOfDay(),
      initialEntryMode: TimePickerEntryMode.input,
    );
    if (selection != null) {
      print(selection.toString());
      setState(() {
        _selectedTime = selection.format(context);
      });
      print(_selectedTime);
    }
  }

  @override
  Widget build(BuildContext context) {
    print('build.');
    print(_selectedTime);
    return Container(
      padding: const EdgeInsets.only(bottom: 45),
      child: InkWell(
        onTap: () {
          _show();
        },
        splashFactory: NoSplash.splashFactory,
        child: TextFormField(
          initialValue: _selectedTime,
          validator: (value) {
            if (value == null || value.isEmpty) {
              return 'Wakeup Time cannot be blank.';
            } else {
              widget.modifiedAlarm.alarmName = value;
              return null;
            }
          },
          enabled: false,
          decoration: InputDecoration(
            disabledBorder: UnderlineInputBorder(
              borderSide: BorderSide(
                // TODO: Figure out exact color to match other input fields.
                color: appTheme.backgroundColor,
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class DeleteAlarmDialog extends StatelessWidget {
  final Alarm serverAlarm;
  const DeleteAlarmDialog({
    Key? key,
    required this.serverAlarm,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
        title: Text('Delete ' + serverAlarm.alarmName + '?'),
        content: const Text('This alarm will be permanently deleted.'),
        actions: [
          TextButton(
            child: const Text('Delete'),
            style: TextButton.styleFrom(
              padding: const EdgeInsets.only(
                right: 30,
              ),
              textStyle: const TextStyle(
                fontSize: 18,
              ),
            ),
            onPressed: () async {
              // await FirestoreService().deleteAlarm(serverAlarm.alarmId);
              // // TODO: Replace this method with one that uses .popUntil('namedRoute')
              // int count = 0;
              // Navigator.of(context).popUntil(
              //   (_) => count++ >= 3,
              // );
            },
          ),
          TextButton(
            child: const Text('Cancel'),
            style: TextButton.styleFrom(
              padding: const EdgeInsets.only(
                right: 25,
              ),
              textStyle: const TextStyle(
                fontSize: 18,
              ),
            ),
            onPressed: () {
              Navigator.of(context).pop();
            },
          ),
        ]);
  }
}

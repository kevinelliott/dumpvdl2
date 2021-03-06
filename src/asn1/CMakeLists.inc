set(ASN_MODULE_SOURCES
	CMAircraftMessage.c
	CMGroundMessage.c
	AircraftFlightIdentification.c
	Airport.c
	AirportDeparture.c
	AirportDestination.c
	APAddress.c
	AEQualifier.c
	AEQualifierVersion.c
	AEQualifierVersionAddress.c
	AirInitiatedApplications.c
	AirOnlyInitiatedApplications.c
	ARS.c
	GroundInitiatedApplications.c
	GroundOnlyInitiatedApplications.c
	CMAbortReason.c
	CMContactRequest.c
	CMContactResponse.c
	CMForwardRequest.c
	CMForwardResponse.c
	CMLogonRequest.c
	CMLogonResponse.c
	CMUpdate.c
	Date.c
	DateTime.c
	DateTimeDepartureETD.c
	Day.c
	FacilityDesignation.c
	LocSysNselTsel.c
	LongTsap.c
	Month.c
	Response.c
	RDP.c
	ShortTsap.c
	Time.c
	Timehours.c
	Timeminutes.c
	VersionNumber.c
	Year.c
	GroundPDUs.c
	UplinkMessage.c
	AircraftPDUs.c
	StartDownMessage.c
	Mode.c
	DownlinkMessage.c
	CPDLCUserAbortReason.c
	CPDLCProviderAbortReason.c
	ProtectedGroundPDUs.c
	ProtectedUplinkMessage.c
	ATCForwardMessage.c
	ForwardHeader.c
	ForwardMessage.c
	ATCForwardResponse.c
	ProtectedAircraftPDUs.c
	ProtectedStartDownMessage.c
	ProtectedMode.c
	ProtectedDownlinkMessage.c
	AlgorithmIdentifier.c
	CPDLCMessage.c
	PMCPDLCUserAbortReason.c
	PMCPDLCProviderAbortReason.c
	ATCUplinkMessage.c
	ATCUplinkMessageData.c
	ATCDownlinkMessage.c
	ATCDownlinkMessageData.c
	ATCMessageHeader.c
	MsgIdentificationNumber.c
	MsgReferenceNumber.c
	LogicalAck.c
	ATCUplinkMsgElementId.c
	ATCDownlinkMsgElementId.c
	AircraftAddress.c
	Altimeter.c
	AltimeterEnglish.c
	AltimeterMetric.c
	ATCUplinkMsgElementIdSequence.c
	ATCDownlinkMsgElementIdSequence.c
	ATISCode.c
	ATSRouteDesignator.c
	ATWAlongTrackWaypoint.c
	ATWAlongTrackWaypointSequence.c
	ATWLevel.c
	ATWLevelSequence.c
	ATWLevelTolerance.c
	ATWDistance.c
	ATWDistanceTolerance.c
	BlockLevel.c
	ClearanceType.c
	Code.c
	CodeOctalDigit.c
	ControlledTime.c
	DateTimeGroup.c
	DegreeIncrement.c
	Degrees.c
	DegreesMagnetic.c
	DegreesTrue.c
	DepartureClearance.c
	DepartureMinimumInterval.c
	Direction.c
	DirectionDegrees.c
	Distance.c
	DistanceKm.c
	DistanceNm.c
	DistanceSpecified.c
	DistanceSpecifiedDirection.c
	DistanceSpecifiedDirectionTime.c
	DistanceSpecifiedKm.c
	DistanceSpecifiedNm.c
	ErrorInformation.c
	Facility.c
	FacilityFunction.c
	FacilityDesignationAltimeter.c
	FacilityDesignationATISCode.c
	FacilityName.c
	Fix.c
	FixName.c
	FlightInformation.c
	FreeText.c
	Frequency.c
	Frequencyhf.c
	Frequencysatchannel.c
	Frequencyuhf.c
	Frequencyvhf.c
	FurtherInstructions.c
	Holdatwaypoint.c
	HoldatwaypointSequence.c
	HoldClearance.c
	Humidity.c
	InterceptCourseFrom.c
	InterceptCourseFromSequence.c
	InterceptCourseFromSelection.c
	Icing.c
	Latitude.c
	LatitudeDegrees.c
	LatitudeDegreesMinutes.c
	LatitudeDegreesMinutesSeconds.c
	LatitudeDirection.c
	LatitudeWholeDegrees.c
	LatitudeLongitude.c
	LatitudeReportingPoints.c
	LatitudeType.c
	LatLonWholeMinutes.c
	LatLonReportingPoints.c
	LegDistance.c
	LegDistanceEnglish.c
	LegDistanceMetric.c
	LegTime.c
	LegType.c
	Level.c
	LevelFeet.c
	LevelFlightLevel.c
	LevelFlightLevelMetric.c
	LevelLevel.c
	LevelMeters.c
	LevelPosition.c
	LevelProcedureName.c
	LevelsOfFlight.c
	LevelSpeed.c
	LevelSpeedSpeed.c
	LevelTime.c
	LevelType.c
	Longitude.c
	LongitudeDegrees.c
	LongitudeDegreesMinutes.c
	LongitudeDegreesMinutesSeconds.c
	LongitudeDirection.c
	LongitudeWholeDegrees.c
	LongitudeReportingPoints.c
	LongitudeType.c
	MinutesLatLon.c
	Navaid.c
	NavaidName.c
	PersonsOnBoard.c
	PlaceBearing.c
	PlaceBearingDistance.c
	PlaceBearingPlaceBearing.c
	Position.c
	PositionDegrees.c
	PositionDistanceSpecifiedDirection.c
	PositionLevel.c
	PositionLevelLevel.c
	PositionLevelSpeed.c
	PositionPosition.c
	PositionProcedureName.c
	PositionReport.c
	PositionRouteClearanceIndex.c
	PositionSpeed.c
	PositionSpeedSpeed.c
	PositionTime.c
	PositionTimeLevel.c
	PositionTimeTime.c
	PositionUnitNameFrequency.c
	Procedure.c
	ProcedureApproach.c
	ProcedureArrival.c
	ProcedureDeparture.c
	ProcedureName.c
	ProcedureTransition.c
	ProcedureType.c
	PublishedIdentifier.c
	RemainingFuel.c
	RemainingFuelPersonsOnBoard.c
	ReportingPoints.c
	RevisionNumber.c
	RouteAndLevels.c
	RouteClearanceSequence.c
	RouteClearance.c
	RouteClearanceIndex.c
	RouteInformation.c
	RouteInformationSequence.c
	RouteInformationAdditional.c
	RTARequiredTimeArrival.c
	RTARequiredTimeArrivalSequence.c
	RTATime.c
	RTATolerance.c
	Runway.c
	RunwayArrival.c
	RunwayDeparture.c
	RunwayDirection.c
	RunwayConfiguration.c
	RunwayRVR.c
	RVR.c
	RVRFeet.c
	RVRMeters.c
	SecondsLatLon.c
	Speed.c
	SpeedIndicated.c
	SpeedIndicatedMetric.c
	SpeedGround.c
	SpeedGroundMetric.c
	SpeedMach.c
	SpeedSpeed.c
	SpeedTime.c
	SpeedTrue.c
	SpeedTrueMetric.c
	SpeedType.c
	SpeedTypeSpeedTypeSpeedType.c
	SpeedTypeSpeedTypeSpeedTypeSpeed.c
	Temperature.c
	TimeLevel.c
	TimeDeparture.c
	TimeDistanceSpecifiedDirection.c
	TimeDistanceToFromPosition.c
	Timehhmmss.c
	TimeUnitNameFrequency.c
	TimePosition.c
	TimePositionLevel.c
	TimePositionLevelSpeed.c
	TimeSeconds.c
	TimeSpeed.c
	TimeSpeedSpeed.c
	TimeTime.c
	TimeToFromPosition.c
	TimeTolerance.c
	ToFrom.c
	ToFromPosition.c
	TrafficType.c
	Turbulence.c
	UnitName.c
	UnitNameFrequency.c
	DMVersionNumber.c
	VerticalChange.c
	VerticalDirection.c
	VerticalRate.c
	VerticalRateEnglish.c
	VerticalRateMetric.c
	WaypointSpeedLevel.c
	WaypointSpeedLevelSequence.c
	WindDirection.c
	Winds.c
	WindSpeed.c
	WindSpeedEnglish.c
	WindSpeedMetric.c
	Fully-encoded-data.c
	PDV-list.c
	Transfer-syntax-name.c
	Presentation-context-identifier.c
	EXTERNALt.c
	ACSE-apdu.c
	AARQ-apdu.c
	AARE-apdu.c
	RLRQ-apdu.c
	RLRE-apdu.c
	ABRT-apdu.c
	ABRT-diagnostic.c
	ABRT-source.c
	ACSE-requirements.c
	Application-context-name-list.c
	Application-context-name.c
	AP-title.c
	AE-qualifier.c
	AP-title-form1.c
	AE-qualifier-form1.c
	AP-title-form2.c
	AE-qualifier-form2.c
	AE-invocation-identifier.c
	AP-invocation-identifier.c
	Associate-result.c
	Associate-source-diagnostic.c
	Association-information.c
	Implementation-data.c
	Mechanism-name.c
	Release-request-reason.c
	Release-response-reason.c
	Authentication-value.c
	Name.c
	RDNSequence.c
	RelativeDistinguishedName.c
	AttributeTypeAndValue.c
	ANY.c
	BOOLEAN.c
	GraphicString.c
	IA5String.c
	INTEGER.c
	NULL.c
	NativeEnumerated.c
	NativeInteger.c
	NumericString.c
	OBJECT_IDENTIFIER.c
	ObjectDescriptor.c
	RELATIVE-OID.c
	asn_SEQUENCE_OF.c
	asn_SET_OF.c
	constr_CHOICE.c
	constr_SEQUENCE.c
	constr_SEQUENCE_OF.c
	constr_SET_OF.c
	OCTET_STRING.c
	BIT_STRING.c
	asn_codecs_prim.c
	ber_tlv_length.c
	ber_tlv_tag.c
	ber_decoder.c
	der_encoder.c
	constr_TYPE.c
	constraints.c
	xer_support.c
	xer_decoder.c
	xer_encoder.c
	per_support.c
	per_decoder.c
	per_encoder.c
	per_opentype.c
)


//
//  AppsFlyerXMacro.h
//  AppsFlyerCocos2dX
//
//  Created by AndreyG-AF on 11/21/17.
//

#ifndef AppsFlyerXMacro_h
#define AppsFlyerXMacro_h

// In app event names constants
#define AFEventLevelAchieved            "af_level_achieved"
#define AFEventAddPaymentInfo           "af_add_payment_info"
#define AFEventAddToCart                "af_add_to_cart"
#define AFEventAddToWishlist            "af_add_to_wishlist"
#define AFEventCompleteRegistration     "af_complete_registration"
#define AFEventTutorial_completion      "af_tutorial_completion"
#define AFEventInitiatedCheckout        "af_initiated_checkout"
#define AFEventPurchase                 "af_purchase"
#define AFEventRate                     "af_rate"
#define AFEventSearch                   "af_search"
#define AFEventSpentCredits             "af_spent_credits"
#define AFEventAchievementUnlocked      "af_achievement_unlocked"
#define AFEventContentView              "af_content_view"
#define AFEventListView                 "af_list_view"
#define AFEventTravelBooking            "af_travel_booking"
#define AFEventShare                    "af_share"
#define AFEventInvite                   "af_invite"
#define AFEventLogin                    "af_login"
#define AFEventReEngage                 "af_re_engage"
#define AFEventUpdate                   "af_update"
#define AFEventOpenedFromPushNotification "af_opened_from_push_notification"
#define AFEventLocation                 "af_location_coordinates"
#define AFEventCustomerSegment          "af_customer_segment"



// In app event parameter names
#define AFEventParamAchievenmentId         "af_achievement_id"
#define AFEventParamLevel                  "af_level"
#define AFEventParamScore                  "af_score"
#define AFEventParamSuccess                "af_success"
#define AFEventParamPrice                  "af_price"
#define AFEventParamContentType            "af_content_type"
#define AFEventParamContentId              "af_content_id"
#define AFEventParamContentList            "af_content_list"
#define AFEventParamCurrency               "af_currency"
#define AFEventParamQuantity               "af_quantity"
#define AFEventParamRegistrationMethod     "af_registration_method"
#define AFEventParamPaymentInfoAvailable   "af_payment_info_available"
#define AFEventParamMaxRatingValue         "af_max_rating_value"
#define AFEventParamRatingValue            "af_rating_value"
#define AFEventParamSearchString           "af_search_string"
#define AFEventParamDateA                  "af_date_a"
#define AFEventParamDateB                  "af_date_b"
#define AFEventParamDestinationA           "af_destination_a"
#define AFEventParamDestinationB           "af_destination_b"
#define AFEventParamDescription            "af_description"
#define AFEventParamClass                  "af_class"
#define AFEventParamEventStart             "af_event_start"
#define AFEventParamEventEnd               "af_event_end"
#define AFEventParamLat                    "af_lat"
#define AFEventParamLong                   "af_long"
#define AFEventParamCustomerUserId         "af_customer_user_id"
#define AFEventParamValidated              "af_validated"
#define AFEventParamRevenue                "af_revenue"
#define AFEventProjectedParamRevenue       "af_projected_revenue"
#define AFEventParamReceiptId              "af_receipt_id"
#define AFEventParamTutorialId             "af_tutorial_id"
#define AFEventParamAchievenmentId         "af_achievement_id"
#define AFEventParamVirtualCurrencyName    "af_virtual_currency_name"
#define AFEventParamDeepLink               "af_deep_link"
#define AFEventParamOldVersion             "af_old_version"
#define AFEventParamNewVersion             "af_new_version"
#define AFEventParamReviewText             "af_review_text"
#define AFEventParamCouponCode             "af_coupon_code"
#define AFEventParamOrderId                "af_order_id"
#define AFEventParam1                      "af_param_1"
#define AFEventParam2                      "af_param_2"
#define AFEventParam3                      "af_param_3"
#define AFEventParam4                      "af_param_4"
#define AFEventParam5                      "af_param_5"
#define AFEventParam6                      "af_param_6"
#define AFEventParam7                      "af_param_7"
#define AFEventParam8                      "af_param_8"
#define AFEventParam9                      "af_param_9"
#define AFEventParam10                     "af_param_10"

#define AFEventParamDepartingDepartureDate  "af_departing_departure_date"
#define AFEventParamReturningDepartureDate  "af_returning_departure_date"
#define AFEventParamDestinationList         "af_destination_list"  //array of string
#define AFEventParamCity                    "af_city"
#define AFEventParamRegion                  "af_region"
#define AFEventParamCountry                 "af_country"


#define AFEventParamDepartingArrivalDate    "af_departing_arrival_date"
#define AFEventParamReturningArrivalDate    "af_returning_arrival_date"
#define AFEventParamSuggestedDestinations   "af_suggested_destinations" //array of string
#define AFEventParamTravelStart             "af_travel_start"
#define AFEventParamTravelEnd               "af_travel_end"
#define AFEventParamNumAdults               "af_num_adults"
#define AFEventParamNumChildren             "af_num_children"
#define AFEventParamNumInfants              "af_num_infants"
#define AFEventParamSuggestedHotels         "af_suggested_hotels" //array of string

#define AFEventParamUserScore               "af_user_score"
#define AFEventParamHotelScore              "af_hotel_score"
#define AFEventParamPurchaseCurrency        "af_purchase_currency"

#define AFEventParamPreferredStarRatings    "af_preferred_star_ratings"    //array of int (basically a tupple (min,max) but we'll use array of int and instruct the developer to use two values)

#define AFEventParamPreferredPriceRange     "af_preferred_price_range"    //array of int (basically a tupple (min,max) but we'll use array of int and instruct the developer to use two values)
#define AFEventParamPreferredNeighborhoods  "af_preferred_neighborhoods" //array of string
#define AFEventParamPreferredNumStops       "af_preferred_num_stops"



#endif /* AppsFlyerXMacro_h */
